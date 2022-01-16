/*
 * @Description: 
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2022-01-16 12:09:07
 * @LastEditors: huang
 * @LastEditTime: 2022-01-17 00:09:55
 */

#include "iomanager.h"
#include "macro.h"
#include "log.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <string.h>
namespace sylar {

    static Logger::ptr g_logger = __LOG_NAME("system");

    IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event) {
        switch(event) {
            case IOManager::READ:
                return read;
            case IOManager::WRITE:
                return write;
            default:
                __ASSERT2(false, "getContext");
        }
    }
    
    void IOManager::FdContext::resetContext(IOManager::FdContext::EventContext& ctx) {
        ctx.scheduler = nullptr;
        ctx.fiber.reset();
        ctx.cb = nullptr;
    }

    void IOManager::FdContext::triggerEvent(IOManager::Event event) {
        __ASSERT(events & event);
        events = (Event)(events & ~event);
        EventContext& ctx = getContext(event);
        if(ctx.cb) {
            ctx.scheduler->schedule(&ctx.cb);
        } else if(ctx.fiber) {
            ctx.scheduler->schedule(&ctx.fiber);
        }
        ctx.scheduler = nullptr;
    }

    IOManager::IOManager(size_t thread, bool use_caller, const std::string& name)
        : Scheduler(thread, use_caller, name) {
        m_epfd = epoll_create(5000);    // epoll fd
        __ASSERT(m_epfd > 0);       

        int rt = pipe(m_tickleFds);     // 开启匿名pipe: 1 read, 2 write
        __ASSERT(rt);

        epoll_event event;
        memset(&event, 0, sizeof(epoll_event));
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = m_tickleFds[0];

        rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);    // 将管道IO设置为非阻塞
        __ASSERT(rt);

        rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);  // 添加读事件event监测
        __ASSERT(rt);

        contextResize(32);
        start();
    }
    IOManager::~IOManager() {
        stop();
        close(m_epfd);
        close(m_tickleFds[0]);
        close(m_tickleFds[1]);
        for(size_t i = 0; i < m_fdContexts.size(); ++i) {
            if(m_fdContexts[i]) {
                delete m_fdContexts[i];
            }
        }
    }
    void IOManager::contextResize(size_t size) {
        m_fdContexts.resize(size);
        for(size_t i = 0; i < m_fdContexts.size(); ++i) {
            if(!m_fdContexts[i]) {
                m_fdContexts[i] = new FdContext;
                m_fdContexts[i]->fd = i;
            }
        }
    }
    // 1 success 0 retry, -1 error
    int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
        FdContext* fd_ctx = nullptr;
        RWMutexType::ReadLock lock(m_mutex);
        if((int)m_fdContexts.size() > fd) {
            fd_ctx = m_fdContexts[fd];                  // 拿出fd所指向的事件内容
            lock.unlock();
        } else {
            lock.unlock();
            RWMutexType::WriteLock lock2(m_mutex);      // 修正事件队列大小，需要加写锁
            contextResize(fd * 1.5);
        }

        FdContext::MutexType::Lock lock2(fd_ctx->mutex);    // 对事件加锁
        if(fd_ctx->events & event) {                    // 不能向已注册的事件加不同类型的句柄，如向读事件添加写事件
            __LOG_ERROR(g_logger) << "addEvent assert fd=" << fd
                                  << " event=" << event
                                  << " fd->m_event" << fd_ctx->events;
            __ASSERT(!(fd_ctx->events & event));
        }

        // 设置epoll_ctl参数
        int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;    // 如果已经有了事件对象，则操作为修改，如果没有，则操作为添加
        Event new_event = (Event)(fd_ctx->events | event);
        epoll_event epevent;                            // 创建新的epoll_event
        epevent.events = EPOLLET | new_event;  // 边缘触发 | 原注册事件的event | 新添加事件的event
        epevent.data.ptr = fd_ctx;

        // 执行epoll_Ctl
        int rt = epoll_ctl(m_epfd, op, fd, &epevent);
        if(rt) {
            __LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                << op << "," << fd << "," << epevent.events << "):"
                << rt << " (" << errno << ") (" << strerror(errno) <<")";
            return -1;
        }

        ++m_pendingEventCount;  // 准备好事件，计数器+1
        fd_ctx->events = new_event;  // 修改vector中存储的event
        FdContext::EventContext& event_ctx = fd_ctx->getContext(event); // 将具体事件的读/写的 调度器/协程/cb方法返回
        __ASSERT(!event_ctx.scheduler
                && !event_ctx.fiber
                && !event_ctx.cb);
        
        event_ctx.scheduler = Scheduler::GetThis(); // 调度器设定为当前线程的调度器
        if(cb) {
            event_ctx.cb.swap(cb);
        } else {
            event_ctx.fiber = Fiber::GetThis();     // 调度器设定为当前线程运行的协程，因为协程正在运行，必为EXEC状态
            __ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
        }

        return 0;
    }
    bool IOManager::delEvent(int fd, Event event) {
        RWMutexType::ReadLock lock(m_mutex);
        if((int)m_fdContexts.size() <= fd) {
            return false;
        }
        
        FdContext* fd_ctx = m_fdContexts[fd];
        lock.unlock();

        FdContext::MutexType::Lock lock2(fd_ctx->mutex);
        if(!(fd_ctx->events & event)) {
            // 若没有对应事件，则返回错误
            return false;
        }

        // 处理epoll_ctl各参数
        Event new_event = (Event) (fd_ctx->events & ~event); // 新事件
        int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epoll_event epevent;
        epevent.events = EPOLLET | new_event;
        epevent.data.ptr = fd_ctx;

        // 执行epollctl
        int rt = epoll_ctl(m_epfd, op, fd, &epevent);
        if(rt) {
            __LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                << op << "," << fd << "," << epevent.events << "):"
                << rt << " (" << errno << ") (" << strerror(errno) <<")";
            return false;
        }

        // 正在监听的事件-1，更改vector状态
        --m_pendingEventCount;
        fd_ctx->events = new_event;
        FdContext::EventContext event_ctx = fd_ctx->getContext(event);
        fd_ctx->resetContext(event_ctx);
        return true;

    }
    bool IOManager::cancelEvent(int fd, Event event) {
        RWMutexType::ReadLock lock(m_mutex);
        if((int)m_fdContexts.size() <= fd) {
            return false;
        }
        
        FdContext* fd_ctx = m_fdContexts[fd];
        lock.unlock();

        FdContext::MutexType::Lock lock2(fd_ctx->mutex);
        if(!(fd_ctx->events & event)) {
            // 若没有所要删除的事件，则返回错误
            return false;
        }

        Event new_event = (Event) (fd_ctx->events & ~event); // 若发现删除事件后，没有需要观测的对象了
        int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epoll_event epevent;
        epevent.events = EPOLLET | new_event;
        epevent.data.ptr = fd_ctx;

        int rt = epoll_ctl(m_epfd, op, fd, &epevent);
        if(rt) {
            __LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                << op << "," << fd << "," << epevent.events << "):"
                << rt << " (" << errno << ") (" << strerror(errno) <<")";
            return false;
        }

        fd_ctx->triggerEvent(event);
        --m_pendingEventCount;
        return true;
    }

    bool IOManager::cancelAll(int fd) {
        RWMutexType::ReadLock lock(m_mutex);
        if((int)m_fdContexts.size() <= fd) {
            return false;
        }
        
        FdContext* fd_ctx = m_fdContexts[fd];
        lock.unlock();

        FdContext::MutexType::Lock lock2(fd_ctx->mutex);
        if(!(fd_ctx->events)) {
            // 若目标事件本身就是0，返回错误
            return false;
        }

        int op = EPOLL_CTL_DEL;
        epoll_event epevent;
        epevent.events = 0;
        epevent.data.ptr = fd_ctx;

        int rt = epoll_ctl(m_epfd, op, fd, &epevent);
        if(rt) {
            __LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                << op << "," << fd << "," << epevent.events << "):"
                << rt << " (" << errno << ") (" << strerror(errno) <<")";
            return false;
        }

        if(fd_ctx->events & READ) {
            fd_ctx->triggerEvent(READ);
            --m_pendingEventCount;
        }
        if(fd_ctx->events & WRITE){
            fd_ctx->triggerEvent(WRITE);
            --m_pendingEventCount;
        }

        __ASSERT(fd_ctx->events == 0);
        return true;
    }

    IOManager* IOManager::GetThis() {
        return dynamic_cast<IOManager*> (Scheduler::GetThis());
    }

    void IOManager::tickle() {
        if(!hasIdleThreads()) {
            return;
        }
        int rt = write(m_tickleFds[1], "T", 1);
        __ASSERT(rt == 1);
    }

    bool IOManager::stopping() {
        return Scheduler::stopping()
            && m_pendingEventCount == 0;
    }

    void IOManager::idle() {
        epoll_event* events = new epoll_event[64]();    // 创建空event对象，方便wait时返回事件
        std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
            delete [] ptr;
        });
        
        while(true) {
            if(stopping()) {
                __LOG_INFO(g_logger) << "name=" << getName() << " idle stopping exit";
                break;
            }
            int rt = 0;
            do {
                static const int MAX_TIMEOUT = 5000;
                int rt = epoll_wait(m_epfd, events, 64, MAX_TIMEOUT);

                if(rt < 0 && errno == EINTR) {
                } else {
                    break;
                }

            } while(true);
            
            for(int i = 0; i < rt; ++i) {
                epoll_event& event = events[i];
                if(event.data.fd == m_tickleFds[0]) {   // 管道需要把数据读取出来，其他事件需要处理
                    uint8_t dummy;
                    while(read(m_tickleFds[0], &dummy, 1) == 1);
                    continue;
                }

                FdContext* fd_ctx = (FdContext*)event.data.ptr;
                FdContext::MutexType::Lock lock(fd_ctx->mutex);

                if(event.events & (EPOLLERR | EPOLLHUP)) {  //错误或者挂断
                    event.events |= EPOLLERR | EPOLLHUP;
                }

                int real_events = NONE;
                if(event.events & EPOLLIN) {
                    real_events |= READ;
                }
                
                if(event.events & EPOLLOUT) {
                    real_events |= WRITE;
                }

                if((fd_ctx->events & real_events) == NONE) {    // 如果实际监测的事件和发生的事件没有交集，则不处理
                    continue;
                }

                int left_events = (fd_ctx->events & ~real_events);  // 剩余事件
                int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;   

                event.events = EPOLLET | left_events;

                int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
                if(rt2) {
                    __LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                        << op << "," << fd_ctx->fd << "," << event.events << "):"
                        << rt2 << " (" << errno << ") (" << strerror(errno) <<")";
                    continue;
                }

                if(real_events & READ) {
                    fd_ctx->triggerEvent(READ);
                    --m_pendingEventCount;
                }

                if(real_events & WRITE) {
                    fd_ctx->triggerEvent(WRITE);
                    --m_pendingEventCount;
                }
            }

            Fiber::ptr cur = Fiber::GetThis();
            auto raw_ptr = cur.get();
            cur.reset();

            raw_ptr->swapOut();
        }
    }
}