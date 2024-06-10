#pragma once
#include<string>
#include<iostream>
#include<muduo/net/EventLoop.h>
#include<unordered_map>
#include"google/protobuf/service.h"

#include<muduo/net/TcpServer.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<google/protobuf/descriptor.h>
//框架提供的专门负责发布rpc服务的网络对象类
class RpcProvider{
public:
    void NotifyService(google::protobuf::Service *service);

    void Run();

private:
    muduo::net::EventLoop m_eventloop;

    struct ServiceInfo
    {
        google::protobuf::Service *m_service;
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;//保存服务方法

    };

    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    void OnConnection(const muduo::net::TcpConnectionPtr &);
    void OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);
    void SendRpcRespons(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);
};