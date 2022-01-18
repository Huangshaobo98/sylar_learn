/*
 * @Description: 
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2022-01-17 16:21:55
 * @LastEditors: huang
 * @LastEditTime: 2022-01-17 16:21:55
 */
#ifndef __FIBER_H__
#define __FIBER_H__
#include <ucontext.h>
#include <memory>
#include <functional>
#include "thread.h"
namespace sylar{
    class Scheduler;

    class Fiber : public std::enable_shared_from_this<Fiber> {
        friend class Scheduler;
        public:
            typedef std::shared_ptr<Fiber> ptr;
            enum State {
                INIT,   // 初始化态
                HOLD,   // 暂停态
                EXEC,   // 执行态
                TERM,   // 结束态
                READY,  // 可执行态
                EXCEPT  // 异常态
            };
        private:
            Fiber();
        public:
            
            Fiber(std::function<void()>cb, size_t stacksize = 0, bool use_caller = false);
            ~Fiber();
            // 重置协程函数，并重置状态
            // INIT, TERM
            void reset(std::function<void()> cb);
            // 切换到当前协程执行
            void swapIn();
            // 切换到后台执行
            void swapOut();
            // 切换为目标协程
            void call();

            void back();

            uint64_t getId() const {return m_id;}

            Fiber::State getState() const { return m_state;}
        public:
            // 设置当前协程
            static void SetThis(Fiber* f);
            // 返回当前执行点的协程
            static Fiber::ptr GetThis();
            // 协程切换到后台，并设置为Ready状态
            static void YieldReady();
            // 协程切换到后台，并设置为Hold状态
            static void YieldToHold();
            // 总协程数
            static uint64_t TotalFibers();
            // 主调用方法，不使用use_caller的方法
            static void MainFunc();
            
            static void CallerMainFunc();

            static uint64_t GetFiberId();
        private:
            uint64_t m_id = 0;
            uint32_t m_stacksize = 0;
            State m_state = INIT;
            ucontext_t m_ctx;
            void* m_stack = nullptr;
            std::function<void()> m_cb;
    };
}

#endif