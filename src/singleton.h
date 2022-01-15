/**
 * @file sigleton.h
 * @brief 单例模式封装
 */


#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <memory>

namespace sylar
{

    /**
     * @brief 未命名命名空间，内部对被声明为静态方法
     */
    namespace
    {
        // 返回单例对象
        template<typename T, typename X, int N>
        T& GetInstance(){
            static T instance;
            return instance;
        }
        
        // 返回单例对象指针
        template<typename T, typename X, int N>
        std::shared_ptr<T> GetInstancePtr(){
            static std::shared_ptr<T> instancePtr(new T);
            return instancePtr;
        }
    }
    
    /**
     * @brief 单例模式封装类
     * @details T类型
     *          X 为了创建多个实例对应的Tag
     *          N 同一个Tag创建多个实例索引
     */
    template<typename T, typename X = void, int N = 0>
    class Singleton
    {
        public:
        /**
         * @brief 返回单例指针
         */
        static T* GetInstance(){
            static T v;
            return &v;
        }

    };
    
    /**
     * @brief 单例模式智能指针封装类
     * @details T类型
     *          X 为了创建多个实例对应的Tag
     *          N 同一个Tag创建多个实例索引
     */
    template<typename T, typename X = void, int N = 0>
    class SingletonPtr
    {
        public:
        /**
         * @brief 返回单例智能指针
         */
        static std::shared_ptr<T> GetInstance(){
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };

}
#endif

