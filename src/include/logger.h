#pragma once
#include"lockqueue.h"
#include<string>

enum LogLevel
{
    INFO,
    ERROR,
};
class Logger{
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel level);
    void Log(std::string msg);

private:
    int m_loglevel;
    LockQueue<std::string> m_lckQueue;
    Logger();
    Logger(const Logger &) = delete;
    Logger(const Logger &&) = delete;
};
#define LOG_INFO(logmsgformat,...)\
    do\
    { \
        Logger &logger=Logger::GetInstance();\
        logger.StLogLevel(INFO);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0)

#define LOG_ERR(logmsgformat,...)\
    do\
    { \
        Logger &logger=Logger::GetInstance();\
        logger.StLogLevel(ERROR);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    }while(0)
    