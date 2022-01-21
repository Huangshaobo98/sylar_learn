#include "hook.h"
#include "fiber.h"
#include "iomanager.h"
#include "log.h"
#include "fdmanager.h"
#include "config.h"
#include <dlfcn.h>
#include <memory>
#include <stdarg.h>



namespace sylar {

    static Logger::ptr g_logger = __LOG_NAME("system");
    static thread_local bool t_hook_enable = false;
    static ConfigVar<int>::ptr g_tcp_connect_timeout = 
        Config::Lookup("tcp.connect.timeout",10000, "tcp connect timeout");

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
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

    static uint64_t s_connect_timeout = -1;
    struct _HookIniter {
        _HookIniter() {
            hook_init();
            s_connect_timeout = g_tcp_connect_timeout->getValue();
            g_tcp_connect_timeout->addListener([](const int& old_value, const int& new_value){
                __LOG_INFO(g_logger) << "tcp connect timeout changed from" << old_value << " to " <<new_value;
                s_connect_timeout = new_value;
            });
        }   
    };

    static _HookIniter s_hook_initer;   // 静态方法，构造函数会先于main执行。

    bool is_hook_enable(){
        return t_hook_enable;
    }

    void set_hook_enable(bool flag) {
        t_hook_enable = flag;
    }

    struct timer_info {
        int cancelled = 0;
    };
    
    template<typename OriginFun, typename ... Args>
    static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name
            ,uint32_t event, int timeout_so, Args&&... args) {
        if(!sylar::t_hook_enable) {
            return fun(fd, std::forward<Args>(args)...);
        }

        __LOG_DEBUG(g_logger) << "do_io<" << hook_fun_name << ">";

        sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);   //fd是非阻塞的
        if(!ctx) {
            return fun(fd, std::forward<Args>(args)...);
        }

        if(ctx->isClosed()) {
            errno = EBADF;
            return -1;
        }

        if(!ctx->isSocket() || ctx->getUserNonblock()) {   // 如非socket或者用户设定的非阻塞态，就什么也不做
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
                        return;
                    }
                    t->cancelled = ETIMEDOUT;
                    iom->cancelEvent(fd, (sylar::IOManager::Event)(event));
                }, winfo);
            }
            // 上次的错误，括号写错了，将后面重试的方法写在了超时情况下，导致其不能切出协程，进行Goto重试
            int rt = iom->addEvent(fd, (sylar::IOManager::Event)(event));
            if(rt) {
                __LOG_ERROR(g_logger) << hook_fun_name << " addEvent("
                    << fd <<", " << event << ")";
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
        return n;
    }
}

extern "C" {
#define XX(name) name ## _fun name ## _f = nullptr; //sleep_fun sleep_f = nullptr;
    HOOK_FUN(XX);           
#undef XX

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

    //socket
    int socket(int domain, int type, int protocol) {
        if(!sylar::t_hook_enable) {
            return socket_f(domain, type, protocol);
        }
        int fd = socket_f(domain, type, protocol);
        if(fd == -1)
            return fd;
        sylar::FdMgr::GetInstance()->get(fd, true);
        return fd;
    }

    int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms) {
        if(!sylar::t_hook_enable) {
            return connect_f(fd, addr, addrlen);
        }
        sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
        if(!ctx || ctx->isClosed()) {
            errno = EBADF;
            return -1;
        }
        if(!ctx->isSocket() || ctx->getUserNonblock()) {
            return connect_f(fd, addr, addrlen);
        }
        int n = connect_f(fd, addr, addrlen);       // 尝试进行一次非阻塞连接
        if(n == 0) {                                // 成功连接， 直接返回
            return 0;
        } else if (n != -1 || errno != EINPROGRESS) {   // 若出现成功连接，errorno不为EINPROGRESS，则表示已经连接完成，直接返回n
            return n;
        }
        // 设置定时器
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        sylar::Timer::ptr timer;
        std::shared_ptr<sylar::timer_info> tinfo(new sylar::timer_info);
        std::weak_ptr<sylar::timer_info> winfo(tinfo);
        
        if(timeout_ms != (uint64_t)-1) {    // 如设定了时间
            timer = iom->addConditionTimer(timeout_ms, [winfo, fd, iom](){ // 添加条件定时器，若winfo存在则执行条件回调函数
                auto t = winfo.lock();
                if(!t || t->cancelled) {    // 确保定时器结束时，timer销毁，同时设定t被取消，也就是说timer超时了
                    return;
                }
                t->cancelled = ETIMEDOUT;   // 确认t销毁
                iom->cancelEvent(fd, sylar::IOManager::WRITE);  // 取消connect的写事件
            }, winfo);
        }

        int rt = iom->addEvent(fd, sylar::IOManager::WRITE);    // 添加写事件
        if(rt == 0) {   // 添加成功
            sylar::Fiber::YieldToHold();    // 任务yield，由epoll负责监听相应端口
            if(timer) {                     // 如果timer存在，调用cancel取消定时器
                timer->cancel();
            }
            if(tinfo->cancelled) {          // 如果取消被设定
                errno = tinfo->cancelled;   // errno会报超时错误
                return -1;
            }
        } else {
            if(timer) {                     // 添加失败那么就取消timer，并且记录错误日志
                timer->cancel();
            }
            __LOG_ERROR(sylar::g_logger) << "connect addEvent(" << fd <<", WRITE) error";
        }
        int error = 0;
        socklen_t len = sizeof(int);        
        if(-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {  // 获取一下当前fd的状态信息
            return -1;
        }
        if(!error) {                        // 正常
            return 0;
        } else {                            // 错误
            errno = error;
            return -1;
        }
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        return connect_with_timeout(sockfd, addr, addrlen, sylar::s_connect_timeout);
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
        int fd = do_io(sockfd, accept_f, "accept", sylar::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
        if(fd >= 0) {
            sylar::FdMgr::GetInstance()->get(fd, true);
        }
        return fd;
    }
    //read
    ssize_t read(int fd, void *buf, size_t count) {
        return do_io(fd, read_f, "read", sylar::IOManager::READ, SO_RCVTIMEO,buf, count);
    }

    ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
        return do_io(fd, readv_f, "readv", sylar::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
    }


    ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
        return do_io(sockfd, recv_f, "recv", sylar::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
    }


    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
        return do_io(sockfd, recvfrom_f, "recvfrom", sylar::IOManager::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
    }

    ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
        return do_io(sockfd, recvmsg_f, "recvmsg", sylar::IOManager::READ, SO_RCVTIMEO, msg, flags);
    }

    // write
    ssize_t write(int fd, const void *buf, size_t count) {
        return do_io(fd, write_f, "write", sylar::IOManager::WRITE, SO_SNDTIMEO, buf, count);
    }

    ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
        return do_io(fd, writev_f, "writev", sylar::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
    }

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
        return do_io(sockfd, send_f, "send", sylar::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
    }

    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
        return do_io(sockfd, sendto_f, "sendto", sylar::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
    }
    
    ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
        return do_io(sockfd, sendmsg_f, "sendmsg", sylar::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
    }

    int close(int fd) {
        if(!sylar::t_hook_enable) {
            return close_f(fd);
        }
        sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
        if(ctx) {
            auto iom = sylar::IOManager::GetThis();
            if(iom) {
                iom->cancelAll(fd);
            }
            sylar::FdMgr::GetInstance()->del(fd);
        }
        return close_f(fd);
    }

    int fcntl(int fd, int cmd, ... /* arg */ ) {
        va_list va;
        va_start(va, cmd);
        switch(cmd) {
            case F_DUPFD:
            case F_DUPFD_CLOEXEC:
            case F_SETFD:
            case F_SETFL:
                {
                    int arg = va_arg(va, int);
                    va_end(va);
                    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
                    if(!ctx || ctx->isClosed() || !ctx->isSocket()) {
                        return fcntl_f(fd, cmd, arg);
                    }
                    ctx->setUserNonblock(arg & O_NONBLOCK);
                    if(ctx->getSysNonblock()) {
                        arg |= O_NONBLOCK;
                    } else {
                        arg &= ~O_NONBLOCK;
                    }
                    return fcntl_f(fd, cmd, arg);
                }
            case F_SETOWN:
            case F_SETSIG:
            case F_SETLEASE:
            case F_NOTIFY:
            case F_SETPIPE_SZ:
                {
                    int arg = va_arg(va, int);
                    va_end(va);
                    return fcntl_f(fd, cmd, arg);
                }
                break;
            case F_GETFD:
            case F_GETFL:
                {
                    va_end(va);
                    int arg = fcntl_f(fd, cmd);
                    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
                    if(!ctx || ctx->isClosed() || !ctx->isSocket()) {
                        return arg;
                    }
                    if(ctx->getUserNonblock()) {
                        return arg | O_NONBLOCK;
                    } else {
                        return arg & ~O_NONBLOCK;
                    }
                }
            case F_GETOWN:
            case F_GETSIG:
            case F_GETLEASE:
            case F_GETPIPE_SZ:
                {
                    va_end(va);
                    return fcntl_f(fd, cmd);
                }
                break;
            case F_SETLK:
            case F_SETLKW:
            case F_GETLK:
                {
                    struct flock* arg = va_arg(va, struct flock*);
                    va_end(va);
                    return fcntl_f(fd, cmd, arg);
                }
                break;
            case F_GETOWN_EX:
            case F_SETOWN_EX:
                {
                    struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                    va_end(va);
                    return fcntl_f(fd, cmd, arg);
                }
                break;
            default:
                va_end(va);
                return fcntl_f(fd, cmd);
        }
    }

    int ioctl(int d, unsigned long int request, ...){
        va_list va;
        va_start(va, request); 
        void* arg = va_arg(va, void*);
        va_end(va);
        if(FIONBIO == request) {
            bool user_nonblock = !!*(int*) arg;
            sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(d);
            if(!ctx || ctx->isClosed() || !ctx->isSocket()) {
                return ioctl_f(d, request, arg);
            }
            ctx->setUserNonblock(user_nonblock);
        }
        return ioctl_f(d, request, arg);

    }

    int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen){
        return getsockopt_f(sockfd, level, optname, optval, optlen);
    }

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen){
        if(!sylar::t_hook_enable) {
            return setsockopt_f(sockfd, level, optname, optval, optlen);
        }
        if(level == SOL_SOCKET) {
            if(optname == SO_RCVTIMEO || optname == SO_RCVTIMEO) {
                sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(sockfd);
                if(ctx) {
                    const timeval* tv = (const timeval*)optval;
                    ctx->setTimeout(optname, tv->tv_sec * 1000 + tv->tv_usec / 1000);
                }
            }
        }
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }

}



