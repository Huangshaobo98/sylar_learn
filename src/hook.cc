#include "hook.h"
#include "fiber.h"
#include "iomanager.h"
#include "log.h"
#include <dlfcn.h>
#include <memory>

namespace sylar {

    static Logger::ptr g_logger = __LOG_NAME("system");
    static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt) 

    void hook_init() {
        static bool is_inited = false;
        if(is_inited) {
            return;
        }
#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);    //sleep_f = (sleep_fun)dlsys(RTLD_NEXT, sleep);
    HOOK_FUN(XX);                                                       //dlsys: 获取指定方法的指针，默认初始化后指向了sleep和usleep等库方法
#undef XX

    }

    struct _HookIniter {
        _HookIniter() {
            hook_init();
        }   
    };

    static _HookIniter s_hook_initer;   // 静态方法，构造函数会先于main执行。

    bool is_hook_enable(){
        return t_hook_enable;
    }

    void set_hook_enable(bool flag) {
        t_hook_enable = flag;
    }
}

extern "C" {
#define XX(name) name ## _fun name ## _f = nullptr; //sleep_fun sleep_f = nullptr;
    HOOK_FUN(XX);           
#undef XX

    struct timer_info {
        int cancelled = 0;

    }
    template<typename OriginFun, typename ... Args>
    static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name
            ,uint32_t event, int timeout_so, Args&&... args) {
        if(!sylar::t_hook_enable) {
            return fun(fd, std::forward<Args>(args)...);
        }
        sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);   //fd是非阻塞的
        if(!ctx) {
            return fun(fd, std::forward<Args>(args)...);
        }

        if(ctx->isClose) {
            error = EBADF;
            return -1;
        }

        if(!ctx->isSocket() || ctx->getUserNonlock()) {   // 如非socket或者用户设定的非阻塞态，就什么也不做
            return fun(fd, std::forward<Args>(args)...);
        }

        uint64_t to = ctx->getTimeout(timeout_so);
        std::shared_ptr<timer_info> tinfo(new timer_info);

    retry:
        ssize_t n = fun(fd, std::forward<Args>(args)...);

        while(n == -1 && errno == EINTR) {              // 信号中断，读或者写失败，重试
            n = fun(fd, std::forward<Args>(args)...);
        }

        if(n == -1 && errno == EAGAIN) {                // 非阻塞error，提示没读到数据，稍后重试
            sylar::IOManager* iom = sylar::IOManager::GetThis();
            sylar::Timer::ptr timer;
            std::weak_ptr<timer_info> winfo(tinfo);     

            if(to != (uint64_t) -1) {   // 有设定超时时间
                timer = iom->addConditionTimer(to, [winfo, fd, iom, event](){
                    auto t = winfo.lock();      // 取出shared_ptr
                    if(!t || t->cancelled) {    // 若t不存在，或者t已经被取消掉了，则不再执行后续
                        return -1;
                    }
                    t->cancelled = ETIMEDOUT;
                    iom->cancelEvent(fd, (sylar::IOManager::Event)(event));
                }, winfo);
                int c = 0;
                uint64_t now = 0;

                int rt = iom->addEvent(fd, (sylar::IOManager::Event)(event));
                if(rt) {
                    __LOG_ERROR(g_logger) << hook_fun_name << " addEvent("
                        << fd <<", " << event << ")";
                    }
                    if(timer) {
                        timer->cancel();
                    }
                    return -1;
                } else {
                    sylar::Fiber::YieldToHold();
                    if(timer) {
                        timer->cancel();
                    }
                    if(tinfo->cancelled) {
                        errno = tinfo->cancelled;
                        return -1;
                    }

                    goto retry;
                }
            }
        }
        return n;
    }

    unsigned int sleep(unsigned int seconds) {
        if(!sylar::t_hook_enable) {
            return sleep_f(seconds);
        }

        sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        iom->addTimer(seconds * 1000, [iom, fiber](){
            iom->schedule(fiber);
        });

        sylar::Fiber::YieldToHold();
        return 0;
    }

    int usleep(useconds_t usec) {
        if(!sylar::t_hook_enable) {
            return usleep_f(usec);
        }

        sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        iom->addTimer(usec / 1000, [iom, fiber](){
            iom->schedule(fiber);
        });
        sylar::Fiber::YieldToHold();
        return 0;
    }

    int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
        if(!sylar::t_hook_enable) {
            return nanosleep_f(rqtp, rmtp);
        }
        int timeout_ms = rqtp->tv_sec * 1000 + rqtp->tv_nsec / 1000 / 1000;
        sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        iom->addTimer(timeout_ms, [iom, fiber](){
            iom->schedule(fiber);
        });
        sylar::Fiber::YieldToHold();
        return 0;
    }

}



typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sleep_f;

typedef int (*usleep_fun) (useconds_t usec);
extern usleep_fun usleep_f;