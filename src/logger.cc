#include"logger.h"
#include<time.h>
#include<iostream>

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{

    std::thread writeLogTask([&](){
        for(;;)
        {
            time_t now=time(nullptr);
            tm *nowtm=localtime(&now);

            char filename[128];
            sprintf(filename,"%d-%d-%d-log.txt",nowtm->tm_year+1900,nowtm->tm_mon+1,nowtm->tm_mday);

            FILE* pf=fopen(filename,"a+");
            if(pf==nullptr)
            {
                std::cout<<"logger file:"<<filename<<" open error!"<<std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg=m_lckQueue.Pop();

            char time_buf[128]={0};
            sprintf(time_buf,"%d:%d:%d => [%s] ",
                    nowtm->tm_hour,
                    nowtm->tm_min,
                    nowtm->tm_sec,
                    (m_loglevel==INFO?"info":"error"));

            msg.insert(0,time_buf);
            msg.append("\n");

            fputs(msg.c_str(),pf);
            fclose(pf);
        }
    });
    writeLogTask.detach();
}


void Logger::StLogLevel(LogLevel level)
{
    m_loglevel=level;
}

//写日志,把日志信息写入LockQueue缓冲区当中
void Logger::Log(std::string msg)
{
    m_lckQueue.Push(msg);
}