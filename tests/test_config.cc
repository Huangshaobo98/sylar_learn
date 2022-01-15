/*
 * @Description: 
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2021-12-29 23:31:06
 * @LastEditors: huang
 * @LastEditTime: 2022-01-10 00:07:39
 */
#include "../src/config.h"
#include "../src/log.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

class Person {
    public:
        std::string m_name = "null";
        int m_age = 0;
        bool m_sex = 0;
        std::string toString() const {
            std::stringstream ss;
            ss << "[Person name=" << m_name
               << " age=" << m_age
               << " sex=" << m_sex
               << "]";
            return ss.str();
        }
        bool operator==(const Person& oth) const {
            return m_name == oth.m_name
                && m_age == oth.m_age 
                && m_sex == oth.m_sex;
        }
};
namespace sylar{
    /**
     * @brief Lexical类型转换偏特化，string 2 set<T>
     * 
     * @tparam T 
     */
    template<>
    class LexicalCast<std::string, Person > {
        public:
            Person operator()  (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                Person p;
                p.m_name = node["name"].as<std::string>();
                p.m_age = node["age"].as<int>();
                p.m_sex = node["sex"].as<bool>();
                return p;
            }
    };

    /**
     * @brief Lexical类型转换偏特化, set<T> 2 string
     * 
     */
    template<>
    class LexicalCast<Person, std::string> {
        public:
            std::string operator()  (const Person& p) {
                YAML::Node node;
                node["name"] = p.m_name;
                node["age"] = p.m_age;
                node["sex"] = p.m_sex;
                std::stringstream ss;
                ss << node;
                return ss.str();
            }
    };
}
sylar::ConfigVar<int>::ptr g_int_value_config = 
    sylar::Config::Lookup("system.port", (int)8080, "system port");

sylar::ConfigVar<float>::ptr g_float_value_config = 
    sylar::Config::Lookup("system.float", (float)3.1415f, "system float");

sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = 
    sylar::Config::Lookup("system.int_vec", std::vector<int> {1, 2}, "system int vec");

sylar::ConfigVar<std::list<int>>::ptr g_int_list_value_config = 
    sylar::Config::Lookup("system.int_list", std::list<int> {3, 4}, "system int list");

sylar::ConfigVar<std::set<int>>::ptr g_int_set_value_config = 
    sylar::Config::Lookup("system.int_set", std::set<int> {3, 4}, "system int set");

sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = 
    sylar::Config::Lookup("system.int_uset", std::unordered_set<int> {3, 4}, "system int uset");

sylar::ConfigVar<std::map<std::string, int> >::ptr g_str_int_map_value_config = 
    sylar::Config::Lookup("system.str_int_map", std::map<std::string, int> {{"k", 2}, {"b", 3}}, "system str int map");

sylar::ConfigVar<std::unordered_map<std::string, int> >::ptr g_str_int_umap_value_config = 
    sylar::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int> {{"k", 2}, {"b", 3}}, "system str int map");

sylar::ConfigVar<Person>::ptr g_person = 
    sylar::Config::Lookup("class.person", Person(), "Insetance person");

sylar::ConfigVar<std::map<std::string, Person> >::ptr g_map_person = 
    sylar::Config::Lookup("class.map", std::map<std::string, Person> {{"cc", Person()}}, "Insetance person");

void print_yaml(const YAML::Node& node, int level) {
    if(node.IsScalar()) {
        __LOG_INFO(__LOG_ROOT) << std::string(level * 2, ' ') << node.Scalar()  << " - " << node.Type() << " - " << level;
    } else if(node.IsNull()) {
        __LOG_INFO(__LOG_ROOT) << std::string(level * 2, ' ') << "NULL - " << node.Type() << " - " << level;
    } else if(node.IsMap()) {
        for(auto it = node.begin();
                it != node.end(); ++it) {
            __LOG_INFO(__LOG_ROOT) << std::string(level * 2, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    } else if(node.IsSequence()) {
        for(size_t i = 0; i < node.size(); ++i) {
                __LOG_INFO(__LOG_ROOT) << std::string(level * 2, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("../log.yml");
    //print_yaml(root, 0);
    __LOG_INFO(__LOG_ROOT) << root;
}

void test_config() {
    __LOG_INFO(__LOG_ROOT) << "before: " << g_int_value_config->getValue();
    __LOG_INFO(__LOG_ROOT) << "before: " << g_float_value_config->getValue();

#define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v) { \
            __LOG_INFO(__LOG_ROOT) << #prefix " " #name << ": " << i; \
        } \
        __LOG_INFO(__LOG_ROOT) << #prefix " " #name " yaml: " << g_var->toString();\
    }

#define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v) { \
            __LOG_INFO(__LOG_ROOT) << #prefix " " #name << ": {" << i.first << " - " << i.second << "}"; \
        } \
        __LOG_INFO(__LOG_ROOT) << #prefix " " #name " yaml: " << g_var->toString();\
    }
    //__LOG_INFO(__LOG_ROOT) << g_int_vec_value_config->toString();
    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    XX_M(g_str_int_map_value_config, str_int_map, before);
    XX_M(g_str_int_umap_value_config, str_int_umap, before);

    YAML::Node root = YAML::LoadFile("../log.yml");
    sylar::Config::LoadFromYaml(root);

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_M(g_str_int_map_value_config, str_int_map, after);
    XX_M(g_str_int_umap_value_config, str_int_umap, after);
#undef XX

}

void test_class() {

    g_person->addListener([](const Person& old_value, const Person& new_value){
        __LOG_INFO(__LOG_ROOT) << "Old_value:\n" << old_value.toString() 
            << "\nNew_value:\n" << new_value.toString();
    });
    //__LOG_INFO(__LOG_ROOT) << "before " << g_person->getValue().toString() << " - " << g_person->toString();
    
    auto m = g_map_person->getValue();
    for(auto& i : m) {
        __LOG_INFO(__LOG_ROOT) << "before: " << i.first << " - " << i.second.toString();
    }

    YAML::Node root = YAML::LoadFile("../log.yml");
    sylar::Config::LoadFromYaml(root);

    //__LOG_INFO(__LOG_ROOT) << "after " << g_person->getValue().toString() << " - " << g_person->toString();

    m = g_map_person->getValue();
    for(auto& i : m) {
        __LOG_INFO(__LOG_ROOT) << "after: " << i.first << " - " << i.second.toString();
    }
}

void test_log() {
    static sylar::Logger::ptr system_log = __LOG_NAME("system");
    __LOG_INFO(system_log) << "hello system 1" <<std::endl;
    std::cout << sylar::LoggerMgr::GetInstance()->toYAMLString() << std::endl;
    YAML::Node root = YAML::LoadFile("../log.yml");
    sylar::Config::LoadFromYaml(root);
    std::cout << "===========================================" << std::endl;
    std::cout << sylar::LoggerMgr::GetInstance()->toYAMLString() << std::endl;
    __LOG_INFO(system_log) << "hello system 2" <<std::endl;
    system_log->setFormatter("%d - %m %n");
    __LOG_INFO(system_log) << "Hello system with another format" << std::endl;
}

void test_visit() {
    sylar::Config::Visit([](sylar::ConfigVarBase::ptr var) {
        __LOG_INFO(__LOG_ROOT) << "name=" << var->getName()
            << " description=" << var->getDescription()
            << " type=" << var->getTypeName()
            << " value=" << var->toString();
    });
}
int main(int argc, char* argv[]) {
    //test_yaml();
    /*test_config();*/
    //test_class();
    //test_log();
    test_visit();
    return 0;
}