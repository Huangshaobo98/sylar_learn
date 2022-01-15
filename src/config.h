/*
 * @Description: 配置系统
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2021-12-29 14:14:57
 * @LastEditors: huang
 * @LastEditTime: 2022-01-09 19:33:13
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <functional>
#include "log.h"
#include "util.h"
#include "mutex.h"

namespace sylar {
    /**
     * @brief 配置变量基类
     */
    class ConfigVarBase {
        public:
            typedef std::shared_ptr<ConfigVarBase> ptr;
            ConfigVarBase(const std::string& name, const std::string& description = "")
                :m_name(name)
                ,m_description(description) {
                    toLower(m_name);
                }
            virtual ~ConfigVarBase() {}

            const std::string& getName() const { return m_name;}
            const std::string& getDescription() const { return m_description;}
            virtual std::string getTypeName() const = 0;
            /**
             * @brief 基类虚方法，将m_val转换为string
             * 
             * @return std::string& 
             */
            virtual std::string toString() = 0;
            /**
             * @brief 基类虚方法，从val转换为m_val的类型
             * 
             * @param val 
             * @return true 
             * @return false 
             */
            virtual bool fromString(const std::string& val) = 0;

        protected:
            std::string m_name;
            std::string m_description;

    };

    /**
     * @brief Lexical类型转换模板
     * 
     * @tparam F From type
     * @tparam T To type
     */
    template<typename F, typename T>
    class LexicalCast {
        public:
            T operator()(const F& v) {
                return boost::lexical_cast<T>(v);
            }
    };

/**
     * @brief Lexical类型转换偏特化，string 2 unordered_map<T>
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::string, std::unordered_map<std::string, T> > {
        public:
            std::unordered_map<std::string, T> operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::unordered_map<std::string, T> vec;
                std::stringstream ss;
                for(auto it = node.begin();
                        it != node.end(); ++it) {
                    ss.str("");
                    ss << it -> second;
                    vec.insert(std::make_pair(it -> first.Scalar(),
                                LexicalCast<std::string, T>()(ss.str())));
                }
                return vec;
            }
    };

    /**
     * @brief Lexical类型转换偏特化, unoredered_map<T> 2 string
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::unordered_map<std::string, T>, std::string> {
        public:
            std::string operator()  (const std::unordered_map<std::string, T>& v) {
                YAML::Node node;
                for(auto& i : v) {
                    node[i.first] = YAML::Load(LexicalCast<T, std::string> ()(i.second));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };


    /**
     * @brief Lexical类型转换偏特化，string 2 map<T>
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::string, std::map<std::string, T> > {
        public:
            std::map<std::string, T> operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::map<std::string, T> vec;
                std::stringstream ss;
                for(auto it = node.begin();
                        it != node.end(); ++it) {
                    ss.str("");
                    ss << it -> second;
                    vec.insert(std::make_pair(it -> first.Scalar(),
                                LexicalCast<std::string, T>()(ss.str())));
                }
                return vec;
            }
    };

    /**
     * @brief Lexical类型转换偏特化, map<T> 2 string
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::map<std::string, T>, std::string> {
        public:
            std::string operator()  (const std::map<std::string, T>& v) {
                YAML::Node node;
                for(auto& i : v) {
                    node[i.first] = YAML::Load(LexicalCast<T, std::string> ()(i.second));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };


    /**
     * @brief Lexical类型转换偏特化，string 2 unordered_set<T>
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::string, std::unordered_set<T> > {
        public:
            std::unordered_set<T> operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::unordered_set<T> vec;
                std::stringstream ss;
                for(size_t i = 0; i < node.size(); ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.insert(LexicalCast<std::string, T>()(ss.str()));
                }
                return vec;
            }
    };

    /**
     * @brief Lexical类型转换偏特化, unordered_set<T> 2 string
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::unordered_set<T>, std::string> {
        public:
            std::string operator()  (const std::unordered_set<T>& v) {
                YAML::Node node;
                for(auto& i : v) {
                    node.push_back(YAML::Load(LexicalCast<T, std::string> ()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };

    /**
     * @brief Lexical类型转换偏特化，string 2 set<T>
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::string, std::set<T> > {
        public:
            std::set<T> operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::set<T> vec;
                std::stringstream ss;
                for(size_t i = 0; i < node.size(); ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.insert(LexicalCast<std::string, T>()(ss.str()));
                }
                return vec;
            }
    };

    /**
     * @brief Lexical类型转换偏特化, set<T> 2 string
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::set<T>, std::string> {
        public:
            std::string operator()  (const std::set<T>& v) {
                YAML::Node node;
                for(auto& i : v) {
                    node.push_back(YAML::Load(LexicalCast<T, std::string> ()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };

    /**
     * @brief Lexical类型转换偏特化，string 2 list<T>
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::string, std::list<T> > {
        public:
            std::list<T> operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::list<T> vec;
                std::stringstream ss;
                for(size_t i = 0; i < node.size(); ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.push_back(LexicalCast<std::string, T>()(ss.str()));
                }
                return vec;
            }
    };

    /**
     * @brief Lexical类型转换偏特化, list<T> 2 string
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::list<T>, std::string> {
        public:
            std::string operator()  (const std::list<T>& v) {
                YAML::Node node;
                for(auto& i : v) {
                    node.push_back(YAML::Load(LexicalCast<T, std::string> ()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };

    /**
     * @brief Lexical类型转换偏特化，string 2 vector<T>
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::string, std::vector<T> > {
        public:
            std::vector<T> operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::vector<T> vec;
                std::stringstream ss;
                for(size_t i = 0; i < node.size(); ++i) {
                    ss.str("");
                    ss << node[i];
                    vec.push_back(LexicalCast<std::string, T>()(ss.str()));
                }
                return vec;
            }
    };
    /**
     * @brief Lexical类型转换偏特化，vector<T> 2 string
     * 
     * @tparam T 
     */
    template<typename T>
    class LexicalCast<std::vector<T>, std::string> {
        public:
            std::string operator()  (const std::vector<T>& v) {
                YAML::Node node;
                for(auto& i : v) {
                    node.push_back(YAML::Load(LexicalCast<T, std::string> ()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };
    
    /**
     * @brief 配置参数模板子类
     * 
     * @tparam T 
     */
    template<typename T, typename FromStr = LexicalCast<std::string, T>, 
                         typename ToStr = LexicalCast<T, std::string>>
    class ConfigVar : public ConfigVarBase {
        public:
            typedef RWMutex RWMutexType;
            typedef std::shared_ptr<ConfigVar> ptr;
            typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

            ConfigVar(const std::string& name, 
                const T& default_value, const std::string description = "")
                :ConfigVarBase(name, description)
                ,m_val(default_value) {
                    
            }
            std::string toString() override {
                try {
                    //return boost::lexical_cast<std::string>(m_val);
                    RWMutexType::ReadLock lock(m_mutex);
                    return ToStr()(m_val);
                } catch(std::exception& e) {
                    __LOG_ERROR(__LOG_ROOT) << "ConfigVar::toString exception"
                        << e.what() <<" convert: " << typeid(m_val).name() << " to string";
                }
                return "";
            }
            /**
             * @brief 将string类型参数val转为目标类型T进行存储
             * 
             * @param val 
             * @return true 
             * @return false 
             */
            bool fromString(const std::string& val) override {
                try {
                    //m_val = boost::lexical_cast<T>(val);
                    //return true;
                    setValue(FromStr()(val));
                } catch(std::exception& e) {
                    __LOG_ERROR(__LOG_ROOT) << "ConfigVar::fromString exception"
                        << e.what() <<" convert: " << typeid(val).name() << " to string";
                }
                return false;
            }
            
            const T getValue() const { 
                RWMutexType::ReadLock lock(m_mutex);
                return m_val;
            }
            void setValue(const T& v) { 
                {
                    //读取数据时加读锁
                    RWMutexType::ReadLock lock(m_mutex);
                    if(v == m_val) {
                        return;
                    }
                    for(auto& it : m_cbs) {
                        it.second(m_val, v);
                    }
                }
                // 写数据时写锁
                RWMutexType::WriteLock lock(m_mutex);
                m_val = v;
            }
            std::string getTypeName() const override { return typeid(T).name();}

            uint64_t addListener(on_change_cb cb) {
                static uint64_t s_fun_id = 0;
                RWMutexType::WriteLock lock(m_mutex);
                ++s_fun_id;
                m_cbs[s_fun_id] = cb;
                return s_fun_id;
            }

            void delListener(uint64_t key) {
                RWMutexType::WriteLock lock(m_mutex);
                m_cbs.erase(key);
            }
            void clearListener() {
                RWMutexType::WriteLock lock(m_mutex);
                m_cbs.clear();
            }
            on_change_cb getListener(uint64_t key) {
                RWMutexType::ReadLock lock(m_mutex);
                auto it = m_cbs.find(key);
                return it == m_cbs.end() ? nullptr : it->second;
            }
        private:
            T m_val;
            mutable RWMutexType m_mutex;
            // 变更回调函数组，uint64_t key，要求唯一，可以用哈希生成
            std::map<uint64_t, on_change_cb> m_cbs;
    };

    class Config {
        public:
            typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
            typedef RWMutex RWMutexType;
            /**
             * @brief 创建和查找名特定配置
             * 
             * @tparam T            模板类型
             * @param name          配置名
             * @param default_value 默认值
             * @param description   描述
             * @return ConfigVar<T>::ptr 
             */
            template<typename T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                const T& default_value, const std::string& description = "") {
                    RWMutexType::WriteLock lock(GetMutex());
                    auto it = GetDatas().find(name);
                    if(it != GetDatas().end()) {
                        // 这里表示发现所要设置的对象名称存在
                        // 需要进一步验证所设置的对象名称的类型是否与与当前类型匹配
                        auto it_ptr = std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
                        if(!it_ptr){
                            __LOG_INFO(__LOG_ROOT) << "Lookup name=" << name << " exits, but its type is "
                                << it->second->getTypeName() << ", not " << typeid(T).name();
                            return nullptr;
                        } else {
                            __LOG_INFO(__LOG_ROOT) << "Lookup name=" << name << " exits.";
                            return it_ptr;
                        }

                    }
                    // 没找到对应的name，则创建
                    if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
                        // find_first_not_of：在name中查找没有出现在字符串abc...789的第一个字符，如果都出现了，则返回npos
                        __LOG_ERROR(__LOG_ROOT) << "lookup name invalid" << name;
                        throw std::invalid_argument(name);
                    }
                    
                    typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
                    GetDatas()[name] = v;
                    return v;
            }
            /**
             * @brief 以类型T和命名name共同寻找相应的
             * 
             * @tparam T 
             * @param name 
             * @return ConfigVar<T>::ptr 
             */
            template<typename T>
            static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
                RWMutexType::ReadLock lock(GetMutex());
                auto it = GetDatas().find(name);
                if(it == GetDatas().end()) {
                    return nullptr;
                }
                // 将基类智能指针转换为子类智能指针
                return std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
            }
            
            static void LoadFromYaml(const YAML::Node& root);
            static ConfigVarBase::ptr LookupBase(const std::string& name);

            static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
        private:
            static ConfigVarMap& GetDatas() {
                static ConfigVarMap s_datas;
                return s_datas;
            } 
            static RWMutexType& GetMutex() {
                static RWMutexType s_mutex;
                return s_mutex;
            }

    };

}

#endif