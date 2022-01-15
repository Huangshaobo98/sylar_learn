#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <semaphore.h>
#include <stdint.h>
#include <atomic>
#include <pthread.h>
#include "mutex.h"
namespace sylar {


    class Semaphore {
        public:
            Semaphore(uint32_t count = 0);
            ~Semaphore();
            void wait();
            void notify();

        private:
            Semaphore(const Semaphore&) = delete;
            Semaphore(const Semaphore&&) = delete;
            Semaphore& operator=(const Semaphore&) = delete;

        private:
            sem_t m_semaphore;
    };

    template<typename T>
    class ScopedLockImpl {
        public:
            ScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
                    m_mutex.lock();
                    m_locked = true;
                }
            ~ScopedLockImpl() {
                m_mutex.unlock();
            }
            void lock() {
                if(!m_locked) {
                    m_mutex.lock();
                    m_locked = true;
                }
            }

            void unlock() {
                if(m_locked){
                    m_mutex.unlock();
                    m_locked = false;
                }
            }

        private:
            T& m_mutex;
            bool m_locked;
    };
    /**
     * @brief 读锁的具体实施
     * 
     * @tparam T 读写锁
     */
    template<typename T>
    class ReadScopedLockImpl {
        public:
            ReadScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
                    m_mutex.rdlock();
                    m_locked = true;
                }
            ~ReadScopedLockImpl() {
                m_mutex.unlock();
            }
            void lock() {
                if(!m_locked) {
                    m_mutex.rdlock();
                    m_locked = true;
                }
            }

            void unlock() {
                if(m_locked){
                    m_mutex.unlock();
                    m_locked = false;
                }
            }

        private:
            T& m_mutex;
            bool m_locked;
    };

    /**
     * @brief 写锁的具体实施
     * 
     * @tparam T 读写锁
     */
    template<typename T>
    class WriteScopedLockImpl {
        public:
            WriteScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
                    m_mutex.wrlock();
                    m_locked = true;
                }
            ~WriteScopedLockImpl() {
                m_mutex.unlock();
            }
            void lock() {
                if(!m_locked) {
                    m_mutex.wrlock();
                    m_locked = true;
                }
            }

            void unlock() {
                if(m_locked){
                    m_mutex.unlock();
                    m_locked = false;
                }
            }

        private:
            T& m_mutex;
            bool m_locked;
    };
    
    class NullMutex {
        public:
            typedef ScopedLockImpl<NullMutex> Lock;
            NullMutex() {}
            ~NullMutex() {}
            void lock() {}
            void unlock() {}
    };


    /**
     * @brief 原子锁
     */
    class CASLock {
    public:
        /// 局部锁
        typedef ScopedLockImpl<CASLock> Lock;

        CASLock() {
            m_mutex.clear();
        }

        ~CASLock() {
        }

        void lock() {
            while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
        }

        void unlock() {
            std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
        }
    private:
        /// 原子状态
        volatile std::atomic_flag m_mutex;
    };

    /**
     * @brief spinlock锁
     */
    class Spinlock {
        public:
            typedef ScopedLockImpl<Spinlock> Lock;
            Spinlock() {
                pthread_spin_init(&m_mutex, 0);
            }
            ~Spinlock() {
                pthread_spin_destroy(&m_mutex);
            }
            void lock() {
                pthread_spin_lock(&m_mutex);
            }
            void unlock() {
                pthread_spin_unlock(&m_mutex);
            }
        private:
            pthread_spinlock_t m_mutex;

    };

    class Mutex {
        public:
            typedef ScopedLockImpl<Mutex> Lock;
        public:
            Mutex() {
                pthread_mutex_init(&m_mutex, nullptr);
            }
            ~Mutex() {
                pthread_mutex_destroy(&m_mutex);
            }
            void lock() {
                pthread_mutex_lock(&m_mutex);
            }
            void unlock() {
                pthread_mutex_unlock(&m_mutex);
            }
        private:
            pthread_mutex_t m_mutex;
    };

    /**
     * @brief 测试用空锁
     * 
     */
    class NullRWMutex {
        public:
            typedef ReadScopedLockImpl<NullRWMutex> ReadLock;
            typedef WriteScopedLockImpl<NullRWMutex> WriteLock;
            NullRWMutex() {}
            ~NullRWMutex() {}
            void rdlock() {}
            void wrlock() {}
            void unlock() {}
    };

    /**
     * @brief 读写锁
     * 
     */
    class RWMutex {
        public:
            typedef ReadScopedLockImpl<RWMutex> ReadLock;
            typedef WriteScopedLockImpl<RWMutex> WriteLock;
        public:
            RWMutex() {
                pthread_rwlock_init(&m_rwlock, nullptr);
            }
            ~RWMutex() {
                pthread_rwlock_destroy(&m_rwlock);
            }
            void rdlock() {
                pthread_rwlock_rdlock(&m_rwlock);
            }
            void wrlock() {
                pthread_rwlock_wrlock(&m_rwlock);
            }
            void unlock() {
                pthread_rwlock_unlock(&m_rwlock);
            }
        private:
            pthread_rwlock_t m_rwlock;
    };

}

#endif