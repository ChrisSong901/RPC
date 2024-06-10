#include"./include/mprpcApplication.h"
#include"./include/rpcprovider.h"
#include"./include/rpcheader.pb.h"
void RpcProvider::NotifyService(google::protobuf::Service* service)
{
     ServiceInfo service_info;//服务表
    //获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc=service->GetDescriptor();
    //获取服务的名字
    std::string service_name=pserviceDesc->name();
    //获取服务对象service的方法数量
    int methodCnt=pserviceDesc->method_count();

    for (int i = 0; i < methodCnt;i++)
    {
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();

        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }

    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().getconfig().Load("rpcserverip");
    uint16_t port =atoi(MprpcApplication::GetInstance().getconfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip,port);
    muduo::net::TcpServer server(&m_eventloop, address, "rpcprovider");

    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);

    std::cout<<"RpcProvider start service at ip:"<<ip<<" port:"<<port<<std::endl;

    server.start();
    m_eventloop.loop();

}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}
//定义header_size（4字节） + header_str + args_str

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buff, muduo::Timestamp)
{
    
    std::string recv_buf = buff->retrieveAllAsString();

    uint32_t head_size = 0;

    recv_buf.copy((char *)&head_size, 4, 0);//读4字节到head_size

    std::string rpc_head_str = recv_buf.substr(4, head_size);

    RPC::RpcHeader rpcheader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(rpcheader.ParseFromString(rpc_head_str))
    {
        service_name = rpcheader.service_name();
        method_name = rpcheader.method_name();
        args_size = rpcheader.args_size();
    }
    else{
        std::cout<<"rpc_header_str: "<<rpc_head_str<<"parse error!"<<std::endl;
        return;
    }
    std::string args_str = recv_buf.substr(4 + head_size, args_size);
    if (m_serviceMap.find(service_name) == m_serviceMap.end())
    {
        //如果方法不存在
        std::cout<<service_name<<"is not exist!"<<std::endl;
        return;
    }
    auto it =m_serviceMap.find(service_name);
    auto mit = it->second.m_methodMap.find(method_name);

    if(mit==it->second.m_methodMap.end())
    {
         std::cout<<service_name<<":"<<method_name<<"is not exists!"<<std::endl;
    }

    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();//生成一个新对象

    if(!request->ParseFromString(args_str))//参数反序列化失败
    {
        std::cout<<"request parse error,content:"<<args_str<<std::endl;
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, 
    const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(this,&RpcProvider::SendRpcRespons,conn,response);
    service->CallMethod(method, nullptr,request, response, done);
}
void RpcProvider::SendRpcRespons(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else{
        std::cout<<"Serialize response error!"<<std::endl;
    }
    //模拟http的短链接服务，由rpcprovider主动断开连接
    conn->shutdown();
}
