#include"./include/mprpcApplication.h"
#include"./include/mprpcconfig.h"
#include<iostream>
#include<string>
#include<unistd.h>
mprpcconfig MprpcApplication::m_config;

void ShowArgHelp()
{
    std::cout<<"format:command -i <configfile>"<<std::endl;
}

void MprpcApplication::init(int argc,char **argv)
{
    int c = 0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:")!=-1))
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout<<"invalid args"<<std::endl;
            ShowArgHelp();
            exit(EXIT_FAILURE);
        case ':':
            std::cout<<"need <configfile>"<<std::endl;
            ShowArgHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    //开始加载配置文件 加载IP，端口，zk的IP和端口
    m_config.Load(config_file);

}
MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}
mprpcconfig& MprpcApplication::getconfig()
{
    return m_config;
}