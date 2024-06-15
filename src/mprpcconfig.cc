#include "mprpcconfig.h"

#include <iostream>
#include <string>
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (nullptr == pf)
    {
        std::cout << config_file << " is not exists" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(pf))
    {
        char buff[512] = {0};
        fgets(buff, 512, pf);

        std::string read_buf(buff);
        Trim(read_buf);

        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }

        int idx = read_buf.find('=');
        if (idx == -1)
        {
            continue;
        }

        std::string key;
        std::string value;

        key = read_buf.substr(0, idx);
        Trim(key);


        int endidx=read_buf.find('\n',idx);
        value = read_buf.substr(idx + 1, endidx - idx-1);
        Trim(value);
        m_configMap.insert({key, value});
    }
}
//查询配置项信息
std::string MprpcConfig::Load(std::string const &key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}
void MprpcConfig::Trim(std::string &src_buf)
{
    //去掉字符串前边多余的空格
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        //说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}