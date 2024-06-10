#pragma once
#include<unordered_map>
#include<string>
//配置文件为：rpcserverip,rpcserverport,zookeeperip,zookeeperport

//框架读取配置文件类
class mprpcconfig
{

public:
     //负责解析加载配置文件
    void LoadConfigFile(const char *file);
    //查询配置项信息
    std::string Load(std::string const &key);

private:
    std::unordered_map<std::string, std::string> m_configMap;
    //去掉字符串前后的空格
    void Trim(std::string &src_buf);
};
