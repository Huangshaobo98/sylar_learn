#include "thread.h"
#include "log.h"
#include "util.h"
#include "mutex.h"
#include <iostream>

namespace sylar{

    static sylar::Logger::ptr g_logger = __LOG_NAME("system");
    static thread_local Thread* t_thread = nullptr; //指向当前线程
    static thread_local std::string t_thread_name = "UNKNOW"; 

    Thread::Thread(std::function<void()> cb, const std::string& name) : m_cb(cb), m_name(name) {
        // 构造函数，用于传入回调方法和线程名
        if(name.empty()) {
            m_name = "UNKNOW";
        }
        // 采用默认attr创建线程，线程号被保存在m_thread中，执行run所指向的方法，其参数为this
        int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);    //将自身的run方法传进去，线程创建成功应返回0
        if(rt) {
            __LOG_ERROR(g_logger) << "pthread_create fail, rt=" << rt
                << " name=" << m_name;
            throw std::logic_error("pthread_create error");
        }
        // 主线程等待子线程准备好了之后才可以继续进行下去，从而可以准备好id等信息。
        m_semaphore.wait();
    }
    
    Thread::~Thread() {
        if(m_thread) {
            // 子线程与主线程分离，子线程结束后，资源自动回收
            pthread_detach(m_thread);
        }

    }

    void Thread::join() {
        if(m_thread) {
            int rt = pthread_join(m_thread, nullptr);
            if(rt) {
                __LOG_ERROR(g_logger) << "pthread_join fail, rt=" << rt
                    << " name=" << m_name;
                throw std::logic_error("pthread_join error");
            }
            m_thread = 0;
        }
    }

    Thread* Thread::GetThis() {
        return t_thread;
    }
    const std::string& Thread::GetName() {
        return t_thread_name;
    }

    void Thread::SetName(const std::string& name) {
        if(t_thread) {
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }

    void* Thread::run(void* arg) {
        // 开启子线程
        Thread* thread = (Thread*) arg;
        // 执行时会将 当前运行的线程指针t_thread指向run的线程
        t_thread = thread;
        thread->m_id = sylar::GetThreadId();
        t_thread_name = thread->m_name;
        /*std::cout << "thread->m_id: " << thread->m_id << std::endl;
        std::cout << "pthread_self()" << pthread_self() << std::endl;*/
        pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

        std::function<void()> cb;
        cb.swap(thread->m_cb);

        // 子线程通知主线程，线程资源准备好，主线程可以继续执行了。
        thread->m_semaphore.notify();

        cb();
        return 0;
    }
}