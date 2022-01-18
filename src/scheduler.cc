#include "scheduler.h"
#include "log.h"
#include "macro.h"
#include "hook.h"
namespace sylar {

    static sylar::Logger::ptr g_logger = __LOG_NAME("system");

    static thread_local Scheduler* t_scheduler = nullptr; // 主协程调度器

    static thread_local Fiber* t_scheduler_fiber = nullptr; // 主协程


    Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) 
        :m_name(name) {
        __ASSERT(threads > 0);
        
        // use_caller表示使用开启调度器的线程作为root线程，这样就少开一个root线程
        // 同时use_caller使用当前线程作为调度器主协程调度器
        if(use_caller) {
            
            sylar::Fiber::GetThis();        // 如果线程没有初始化协程，则初始化一个协程        
            --threads;                      // 使用一个线程来调度协程，因此threads-1;
    
            __ASSERT(GetThis() == nullptr); // 一个线程只能创建一个协程调度器，因此GetThis只能为空
            t_scheduler = this;             // 主调度器指向自身
            
            m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));    // 创建调度器线程的主协程，这个协程运行在管理线程下
                                                                                        // 不是0号协程了，至少为1号协程
            sylar::Thread::SetName(m_name); // 给本线程设定name

            t_scheduler_fiber = m_rootFiber.get();    // 主协程被保存在thread_local变量中，这里也不是0号协程
            m_rootThread = sylar::GetThreadId();    // 管理线程的id，也就是schedulerId
            m_threadIds.push_back(m_rootThread);    // 线程容器，存储各线程id

        } else {
            m_rootThread = -1;  // 不使用调度器线程作为root线程
        }

        m_threadCount = threads;    //记录线程数量
    }

    Scheduler::~Scheduler() {
        // 调度器析构
        __ASSERT(m_stopping);
        if(GetThis() == this) {
            t_scheduler = nullptr;
        }
    }

    Scheduler* Scheduler::GetThis() {
        return t_scheduler;
    }
    Fiber* Scheduler::GetMainFiber() {
        return t_scheduler_fiber;
    }

    void Scheduler::start() {
        // 线程池，启动各线程
        MutexType::Lock lock(m_mutex);
        if(!m_stopping) {
            // 如果没启动则返回
            // 默认情况下是1，也就是stop状态，不可重复开启
            return;
        }
        m_stopping = false;
        __ASSERT(m_threads.empty());

        m_threads.resize(m_threadCount);
        for(size_t i = 0; i < m_threadCount; ++i) {
            m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), m_name + "_" + std::to_string(i)));
            m_threadIds.push_back(m_threads[i]->getId());
        }

        lock.unlock();
        
        /*if(m_rootFiber) {
            m_rootFiber->call();
            __LOG_INFO(g_logger) <<"call out";
        }*/

    }
    void Scheduler::stop() {
        m_autoStop = true;
        // 主协程存在，且当前线程数为0(不包括Scheduler所在的协程)，同时主协程状态为TERM或INIT，需要将stopping状态位置0
        if(m_rootFiber && m_threadCount == 0 && 
                (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)) {
            __LOG_INFO(g_logger) << "Secheduler: " << this << " stopped";
            m_stopping = true;
        }
        if(stopping()) {
            // 只有一个线程的情况，从这里返回了
            return;
        }

        //bool exit_on_this_fiber = false;

        __LOG_INFO(g_logger) << "Secheduler::GetThis:" << GetThis() << " This:" << this;

        if(m_rootThread != -1) {
            // use_caller = true 当前线程指向的调度器必须为主scheduler
            __ASSERT(GetThis() == this);
        } else {
            // use_caller = false 没设置t_scheduler，因此为0
            __ASSERT(GetThis() != this);
        }
        m_stopping = true;
        for(size_t i = 0; i < m_threadCount; ++i) {
            tickle();   // 类似信号量，唤醒线程，告知线程结束自己。
        }

        if(m_rootFiber) {
            tickle();   // 唤醒主线程中的协程
        }

        // 如果主协程存在，需要判断是否停止
        if(m_rootFiber) {
            /*while(!stopping()) {
                if(m_rootFiber->getState() == Fiber::TERM
                    || m_rootFiber->getState() == Fiber::EXCEPT) {
                        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
                    __LOG_INFO(g_logger) << " root fiber is term, reset";
                    t_fiber = m_rootFiber.get();
                }
                m_rootFiber->call();
            }*/

            //若没停止，则继续调用主协程
            if(!stopping()) {
                m_rootFiber->call();
            }
            
        }

        // 回收其他线程
        std::vector<Thread::ptr> thrs;
        {
            MutexType::Lock lock(m_mutex);
            thrs.swap(m_threads);
        }

        for(auto& i : thrs) {
            i->join();
        }
    }
    
    void Scheduler::setThis() {
        t_scheduler = this;
    }

    void Scheduler::run(){
        // 1号及以后的协程执行的操作
        __LOG_INFO(g_logger) << "run";

        set_hook_enable(true);
        setThis();  // 先把主调度器设为自身
        if(sylar::GetThreadId() != m_rootThread) {    // 如果线程ID不为主线程ID，也就是说子线程
            t_scheduler_fiber = Fiber::GetThis().get();   // 当前协程就设为Fiber的主协程
        }
        Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));    //构造一个用于处理闲置状态的协程
        Fiber::ptr cb_fiber;

        Assign ft;
        while(true) {
            ft.reset();
            bool is_active = false;
            bool tickle_me = false;
            {
                MutexType::Lock lock(m_mutex);
                // 从协程队列中取出协程
                auto it = m_fibers.begin();     // it 指向 消息队列头部
                while(it != m_fibers.end()) {
                    if(it->thread != -1 && it->thread != sylar::GetThreadId()) {
                        // 如果已经指定好了线程，且当前帧不等于它指定的线程，就不要处理它
                        ++it;
                        // 唤醒位，需要唤醒其他线程来执行任务
                        tickle_me = true;
                        continue;
                    }

                    __ASSERT(it->fiber || it->cb);
                    if(it->fiber && it->fiber->getState() == Fiber::EXEC) {
                        // 所指向的fiber正在执行任务，也不需要处理
                        ++it;
                        continue;
                    }
                    // 此时才是thread真正可以处理的fiber，需要将其拿出来，并从fiber序列中移除
                    ft = *it;
                    m_fibers.erase(it);
                    ++m_activeThreadCount;
                    is_active = true;
                    break;
                }
                tickle_me |= it != m_fibers.end();
            }
            if(tickle_me) {
                // 标志位，唤醒其他线程
                tickle();
            }

            if(ft.fiber && (ft.fiber->getState() != Fiber::TERM
                        && ft.fiber->getState() != Fiber::EXCEPT)) {
                // 切入协程
                
                ft.fiber->swapIn();
                --m_activeThreadCount;
                if(ft.fiber->getState() == Fiber::READY) {
                    // 这里表示协程结束后进入准备状态，就再次将其进行调度
                    schedule(ft.fiber);
                } else if (ft.fiber->getState() != Fiber::TERM 
                    && ft.fiber->getState() != Fiber::EXCEPT) {
                        // 状态不为结束态，也不为异常态，表示让出了CPU
                    ft.fiber->m_state = Fiber::HOLD;
                }
                ft.reset();
            } else if (ft.cb) {
                if(cb_fiber) {
                    // 如果cb_fiber已经指向了协程，只需要更改协程里的回调方法即可
                    cb_fiber->reset(ft.cb);
                } else {
                    // 如果cb_fiber没有任何指向，那么需要创建协程
                    cb_fiber.reset(new Fiber(ft.cb));
                }
                ft.reset();

                cb_fiber->swapIn();
                --m_activeThreadCount;

                if(cb_fiber->getState() == Fiber::READY) {
                    // 这里表示协程结束后进入准备状态，就再次将其进行调度
                    schedule(cb_fiber);
                    cb_fiber.reset();
                } else if (cb_fiber->getState() == Fiber::TERM 
                    || cb_fiber->getState() == Fiber::EXCEPT) {
                        // 状态为结束态，或异常态，表示退出线程
                    cb_fiber->reset(nullptr);
                } else /*if (cb_fiber->getState() != Fiber::TERM)*/ {
                    cb_fiber->m_state= Fiber::HOLD;
                    cb_fiber.reset();
                }
            } else {
                if(is_active) {
                    --m_activeThreadCount;
                    continue;
                }
                if(idle_fiber->getState() == Fiber::TERM) {
                    __LOG_INFO(g_logger) << "idle fiber term";
                    //idle_fiber.reset();
                    break;
                }
                ++m_idleThreadCount;
                idle_fiber->swapIn();
                --m_idleThreadCount;

                if(idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
                    idle_fiber->m_state = Fiber::HOLD;
                }        
            }
        }
    }
    void Scheduler::tickle() {
        __LOG_INFO(g_logger) << "tickle";
    }
    bool Scheduler::stopping() {
        MutexType::Lock lock(m_mutex);
        // 判断调度器可停止条件：自动停止位为1, stopping状态位为1, 协程队列为空, 活跃状态的线程为0
        return m_autoStop && m_stopping && m_fibers.empty() && m_activeThreadCount == 0;
    }
    void Scheduler::idle() {
        __LOG_INFO(g_logger) << "idle";
        while(!stopping()) {
            sylar::Fiber::YieldToHold();
        }
    }
}