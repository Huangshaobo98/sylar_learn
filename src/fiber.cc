#include "fiber.h"
#include "config.h"
#include "macro.h"
#include "scheduler.h"
#include <atomic>
#include <ucontext.h>

namespace sylar {
    static std::atomic<uint64_t> s_fiber_id {0};
    static std::atomic<uint64_t> s_fiber_count {0};
    
    static Logger::ptr g_logger = __LOG_NAME("system");
    static thread_local Fiber* t_fiber = nullptr;           // 当前协程
    static thread_local Fiber::ptr t_threadFiber = nullptr; // 线程的主协程

    static ConfigVar<uint32_t>::ptr g_fiber_stack_size = 
        Config::Lookup<uint32_t>("fiber.stack_size", 1024 * 1024, "fiber stack size");

    class MallocStackAllocator {
        public:
            static void* Alloc(size_t size) {
                return malloc(size);
            }

            static void Dealloc(void* vp, size_t size) {
                return free(vp);
            }
    };

    using StackAllocator = MallocStackAllocator;
    
    void Fiber::SetThis(Fiber* f) {
        t_fiber = f;
    }

    Fiber::ptr Fiber::GetThis() {
        // 两个工作：
        // 1. 若当前协程存在，返回
        // 2. 若当前协程不存在，创建主协程，并执行主协程
        if(t_fiber) {
            // 若存在当前执行的协程，则返回其智能指针
            return t_fiber->shared_from_this();
        }
        // 若不存在，则创建主协程，其中主协程创建时会调用SetThis，将main_fiber设置为t_fiber
        Fiber::ptr main_fiber(new Fiber);
        __ASSERT(t_fiber == main_fiber.get());
        t_threadFiber = main_fiber;
        return t_fiber->shared_from_this();
    }
    
    void Fiber::YieldReady() {
        // 协程切换到后台，并设置为Ready状态
        Fiber::ptr cur = GetThis();
        cur->m_state = READY;
        cur->swapOut();
    }
    
    void Fiber::YieldToHold() {
        // 协程切换到后台，并设置为Hold状态
        Fiber::ptr cur = GetThis();
        cur->m_state = HOLD;
        cur->swapOut();

    }
    // 总协程数
    uint64_t Fiber::TotalFibers() {
        return s_fiber_count;
    }
    // 
    void Fiber::MainFunc() {
        Fiber::ptr cur = GetThis();
        __ASSERT(cur);
        try {
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        } catch (std::exception& ex) {
            cur->m_state = EXCEPT;
            __LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
                << " fiber_id=" << cur->getId()
                << std::endl
                << sylar::BacktraceToString();
        } catch (...) {
            cur->m_state = EXCEPT;
            __LOG_ERROR(g_logger) << "Fiber Except"
                << std::endl
                << sylar::BacktraceToString();
        }
        // 这里取出裸指针，直接用裸指针来控制协程切出
        auto raw_ptr = cur.get();
        cur.reset();    
        raw_ptr->swapOut();
        //程序不会执行到这里，因此无法释放智能指针包裹的cur
        __ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));
    }
    void Fiber::CallerMainFunc() {
        Fiber::ptr cur = GetThis();
        __ASSERT(cur);
        try {
            cur->m_cb();
            cur->m_cb = nullptr;
            cur->m_state = TERM;
        } catch (std::exception& ex) {
            cur->m_state = EXCEPT;
            __LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
                << " fiber_id=" << cur->getId()
                << std::endl
                << sylar::BacktraceToString();
        } catch (...) {
            cur->m_state = EXCEPT;
            __LOG_ERROR(g_logger) << "Fiber Except"
                << " fiber_id=" << cur->getId()
                << std::endl
                << sylar::BacktraceToString();
        }

        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->back();
        __ASSERT2(false, "never reach fiber_id=" + std::to_string(raw_ptr->getId()));

    }

    uint64_t Fiber::GetFiberId() {
        if(t_fiber) {
            return t_fiber->getId();
        }
        return 0;
    }
    Fiber::Fiber() {
        // 只允许在每个线程创建主协程时进入
        m_state = EXEC;
        SetThis(this);

        if(getcontext(&m_ctx)) {
            __ASSERT2(false, "getcontext");
        }

        ++s_fiber_count;
        __LOG_DEBUG(g_logger) << "Fiber::Fiber main";
    }

    Fiber::Fiber(std::function<void()>cb, size_t stacksize, bool use_caller) 
        :m_id(++s_fiber_id)
        ,m_cb(cb){
        ++s_fiber_count;
        m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

        m_stack = StackAllocator::Alloc(m_stacksize);
        if(getcontext(&m_ctx)) {
            __ASSERT2(false, "getcontext");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;
        // 以static方法MainFunc作为入参地址，make出上下文，然后存储在协程实例的m_ctx中
        if(!use_caller) {
            makecontext(&m_ctx, &Fiber::MainFunc, 0);
        } else {
            makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
        }
        
        __LOG_DEBUG(g_logger) << "Fiber::Fiber id=" << m_id;
    }
    Fiber::~Fiber() {
        --s_fiber_count;
        if(m_stack) {
            __ASSERT(m_state == TERM
                || m_state == INIT
                || m_state == EXCEPT);    //断言处于TERM/INIT/EXCEPT态
            StackAllocator::Dealloc(m_stack, m_stacksize);
        } else {
            __ASSERT(!m_cb);    //断言无callback
            __ASSERT(m_state == EXEC);  //断言处于执行态
            Fiber* cur = t_fiber;   //cur指向正在执行的协程
            if(cur == this) {
                SetThis(nullptr);
            }
        }
        __LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
    }
    // 重置协程函数，并重置状态
    // INIT, TERM
    void Fiber::reset(std::function<void()> cb) {

        __ASSERT(m_stack);
        // 状态必须是TERM终止态，或者INIT初始化态
        __ASSERT(m_state == TERM
                || m_state == INIT
                || m_state == EXCEPT);
        m_cb = cb;
        if(getcontext(&m_ctx)) {
            __ASSERT2(false, "getcontext");
        }
        m_ctx.uc_link = nullptr;
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;

        makecontext(&m_ctx, &Fiber::MainFunc, 0);
        m_state = INIT;
        
    }

    void Fiber::back() {
        SetThis(t_threadFiber.get());   // 调入主协程
        if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)){     // 切出本协程，将状态量保存在本协程中
            __ASSERT2(false, "swapcontext");
        }
    }

    void Fiber::call() {
        SetThis(this);  // 调入当前协程
        m_state = EXEC;
        if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {    // 切换进本协程，将状态量保存到主协程中
            __ASSERT2(false, "swapcontext");
        }
    }

    // 切换到当前协程执行
    void Fiber::swapIn() {
        // 设置this为当前协程
        SetThis(this);
        __ASSERT(m_state != EXEC);

        // 将当前协程设为执行态
        m_state = EXEC;
        // 采用调度器的方法，切入本协程，将状态量保存到调度器主协程中
        if(swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx)) {
            __ASSERT2(false, "swapcontext");
        }
    }
    // 切出
    void Fiber::swapOut() {

        SetThis(Scheduler::GetMainFiber()); // 返回调度器主协程
        if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)){ //这里Scheduler主协程跟自己切换了
            __ASSERT2(false, "swapcontext");
        }

    }
}