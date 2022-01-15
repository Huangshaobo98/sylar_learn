/*
 * @Description: 
 * @Version: 1.0
 * @Autor: huang
 * @Date: 2021-12-29 23:33:22
 * @LastEditors: huang
 * @LastEditTime: 2022-01-09 19:34:18
 */
#include "config.h"

namespace sylar
{
    //Config::ConfigVarMap Config::s_datas;
    /**
     * @brief 以递归形式列出yaml中所有成员元素
     * 
     * @param[in] prefix 
     * @param[in] node 
     * @param[out] output 
     */
    static void ListAllMember(const std::string& prefix,
                          const YAML::Node& node,
                          std::list<std::pair<std::string, const YAML::Node> >& output) {
        if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != std::string::npos) {
            // 遇到不合法情况，直接返回
            __LOG_ERROR(__LOG_ROOT) << "Config invalid name: " << prefix << " : " << node;
            return;
        }
        output.push_back(std::make_pair(prefix, node));
        if(node.IsMap()) {
            for(auto it = node.begin(); 
                    it != node.end(); ++it) {
                ListAllMember(prefix.empty() ? it->first.Scalar()
                    : prefix + "." + it->first.Scalar(), it->second, output);
            }
        }
    }

    ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
        auto it = GetDatas().find(name);
        return it == GetDatas().end() ? nullptr : it->second;
    }

    void Config::LoadFromYaml(const YAML::Node& root) {
        std::list<std::pair<std::string, const YAML::Node> > all_nodes;
        // 将所有YAML读取到的节点信息存储在all_nodes中
        ListAllMember("", root, all_nodes);

        for(auto& i : all_nodes) {
            std::string key = i.first;
            // 空key跳过
            if(key.empty()) {
                continue;
            }

            toLower(key);
            // 从base类中查找是否有相应的key对象
            ConfigVarBase::ptr var = LookupBase(key);

            if(var) {
                if(i.second.IsScalar()) {
                    // 设定相应的参数
                    var->fromString(i.second.Scalar());
                } else {
                    std::stringstream ss;
                    ss << i.second;
                    var->fromString(ss.str());
                }
            }
        }
    }
    void Config::Visit(std::function<void(ConfigVarBase::ptr)> cb) {
        //通过回调函数cb遍历每一个参数
        RWMutexType::ReadLock lock(GetMutex());
        ConfigVarMap& m = GetDatas();
        for(auto it = m.begin(); it != m.end(); ++it) {
            cb(it->second);
        }
    }

} // namespace sylar
