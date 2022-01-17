/*
 * @Description: 
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2022-01-16 11:39:52
 * @LastEditors: huang
 * @LastEditTime: 2022-01-17 16:37:12
 */
#ifndef __IOMANAGER_H__
#define __IOMANAGER_H__
#include "mutex.h"
#include "scheduler.h"
#include "fiber.h"
#include <functional>
#include <vector>
#include <sys/epoll.h>
namespace sylar{
    class IOManager : public Scheduler {
        public:
            std::shared_ptr<IOManager> ptr;
            typedef RWMutex RWMutexType;

            enum Event {
                NONE    = 0x0,
                READ    = 0x1,
                WRITE   = 0x4,
            };
        
        private:
            struct FdContext {
                typedef Mutex MutexType;
                struct EventContext {
                    Scheduler* scheduler = nullptr; // 待执行 scheduler
                    Fiber::ptr fiber;               // 事件协程
                    std::function<void()> cb;       // 事件回调函数
                };
                EventContext& getContext(Event event);
                void resetContext(EventContext& ctx);
                void triggerEvent(Event event);
                EventContext read;      // 读事件
                EventContext write;     // 写事件
                int fd;                 // 事件关联的句柄
                Event events = NONE;   // 已经注册的事件
                MutexType mutex;        
            };

        public:
            IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
            ~IOManager();
            
            // 1 success 0 retry, -1 error
            int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
            bool delEvent(int fd, Event event);
            bool cancelEvent(int fd, Event event);

            bool cancelAll(int fd);

            static IOManager* GetThis();
        
        protected:

            void tickle() override;
            bool stopping() override;
            void idle() override;

            void contextResize(size_t size); 
        private:
            int m_epfd = 0;
            int m_tickleFds[2];

            std::atomic<size_t> m_pendingEventCount = {0};
            RWMutexType m_mutex;
            std::vector<FdContext*> m_fdContexts;   // 关注的事件清单
    };
}

#endif