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