#pragma once
#include"./mprpcconfig.h"
class MprpcApplication
{
public:
    static void init(int argc, char **argv);
    static MprpcApplication &GetInstance();
    static mprpcconfig& getconfig();

private:
    static mprpcconfig m_config;
    MprpcApplication(){};
    MprpcApplication(const MprpcApplication &)=delete;
    MprpcApplication& operator=(const MprpcApplication &)=delete;
    MprpcApplication(const MprpcApplication &&)=delete;
};


