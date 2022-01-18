#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <memory>
#include <vector>
#include <list>
#include "fiber.h"

namespace sylar {
    
    class Scheduler {
        public:
            typedef std::shared_ptr<Scheduler> ptr;
            typedef Mutex MutexType;

            /**
             *  @brief 调度器构造函数
             *  @param threads 线程数
             *  @param use_caller 是否使用多线程调度
             *  @param name 调度器线程名
             */
            Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "Scheduler");
            ~Scheduler();
            const std::string& getName() const { return m_name;}
            
            static Scheduler* GetThis();
            static Fiber* GetMainFiber();

            void start();
            void stop();

            /**
             *  @brief 执行调度
             *  @param fc 协程或者回调方法
             *  @param thread 指定线程
             */
            template<class FiberOrCallback>
            void schedule(FiberOrCallback fc, int thread = -1) {
                bool need_tickle = false;
                {
                    MutexType::Lock lock(m_mutex);
                    need_tickle = scheduleNoLock(fc, thread);
                }

                if(need_tickle) {
                    tickle();
                }
                

            }
            template<class InputIterator>
            void schedule(InputIterator begin, InputIterator end) {
                bool need_tickle = false;
                {
                    MutexType::Lock lock(m_mutex);
                    while(begin != end) {
                        need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                        ++begin;
                    }
                }
                if(need_tickle) {
                    tickle();
                }
            }
            
        protected:
            virtual void tickle();
            void run();
            virtual bool stopping();
            virtual void idle();    // 闲置处理，即没有事情做但又不能使线程终止

            void setThis();
            bool hasIdleThreads() { return m_idleThreadCount > 0;}
        private:
            template<class FiberOrCallback>
            bool scheduleNoLock(FiberOrCallback fc, int thread = -1) {
                // 这里的意思是，若出现某个协程需要执行
                // 而此时协程队列m_fibers中没有队列，则通知线程可以取协程执行
                bool need_tickle = m_fibers.empty();
                Assign task(fc, thread);
                if(task.fiber || task.cb) {
                    m_fibers.push_back(task);
                }
                return need_tickle;
            }
        private:
            struct Assign {
                Fiber::ptr fiber;
                std::function<void()> cb;
                int thread; //用于指定协程在哪个线程执行
                
                // 指定协程和线程
                Assign(Fiber::ptr f, int thr)
                    :fiber(f), thread(thr) {

                }

                // 指定协程和线程，同时销毁输入协程指针
                Assign(Fiber::ptr* f, int thr)
                    :thread(thr) {
                    fiber.swap(*f);
                }

                // 指定回调方法
                Assign(std::function<void()> f, int thr)
                    :cb(f), thread(thr) {

                }

                // 指定回调方法，同时销毁回调指针
                Assign(std::function<void()>* f, int thr) 
                    :thread(thr) {
                        cb.swap(*f);
                }

                // 不指定任何线程
                Assign() 
                    :thread(-1) {
                }

                void reset() {
                    fiber = nullptr;
                    cb = nullptr;
                    thread = -1;
                }
            };
        private:
            
            mutable MutexType m_mutex;  
            
            std::vector<Thread::ptr> m_threads; // 线程池
            std::list<Assign> m_fibers; // 保存将要执行的协程(及其指定的线程)
            Fiber::ptr m_rootFiber; //主协程
            std::string m_name;
        protected:
            std::vector<int> m_threadIds; // 保存线程id
            size_t m_threadCount = 0;   //线程数量
            std::atomic<size_t> m_activeThreadCount = {0}; //活跃线程数
            std::atomic<size_t> m_idleThreadCount = {0};    //空闲线程数
            bool m_stopping = true;    // 调度器停止状态
            bool m_autoStop = false;    // 是否自动停止
            int m_rootThread = 0;   //主线程Id
    };
}

#endif