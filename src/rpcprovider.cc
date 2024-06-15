#include"rpcprovider.h"
#include"mprpcapplication.h"
#include"rpcheader.pb.h"
#include"zookeeperutil.h"


void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;//服务表
    //获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc=service->GetDescriptor();
    //获取服务的名字
    std::string service_name=pserviceDesc->name();
    //获取服务对象service的方法数量
    int methodCnt=pserviceDesc->method_count();

    LOG_INFO("service_name:%s",service_name.c_str());

    for(int i=0;i<methodCnt;++i)
    {

        const google::protobuf::MethodDescriptor* pmethodDesc=pserviceDesc->method(i);
        std::string method_name=pmethodDesc->name();

        service_info.m_methodMap.insert({method_name,pmethodDesc});
        LOG_INFO("method_name:%s",method_name.c_str());
    }
    service_info.m_service=service;
    m_serviceMap.insert({service_name,service_info});
}


void RpcProvider::Run()
{
    std::string ip=MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);


    muduo::net::TcpServer server(&m_eventLoop,address,"RpcProvider");
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessge,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    server.setThreadNum(4);

    //把当前rpc节点上要发布的服务全部注册在zk上，让rpc client可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();
    for(auto &sp:m_serviceMap)
    {
    
        std::string service_path ="/"+sp.first;
        zkCli.Create(service_path.c_str(),nullptr,0);//创建临时性节点
        for(auto &mp:sp.second.m_methodMap)
        {
            //service_name/method_name
            std::string method_path=service_path+"/"+mp.first;//拼接服务器路径和方法路径
            char method_path_data[128]={0};
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);//向data中写入路径

            //创建节点,ZOO_EPHEMERAL表示临时节点
            zkCli.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);
        }
    }

    std::cout<<"RpcProvider start service at ip:"<<ip<<" port:"<<port<<std::endl;

    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}

void RpcProvider::OnMessge(const muduo::net::TcpConnectionPtr &conn,
                                    muduo::net::Buffer *buffer,
                                    muduo::Timestamp)
{

    std::string recv_buf=buffer->retrieveAllAsString();

    uint32_t header_size=0;
    recv_buf.copy((char*)&header_size,4,0);
    std::string rpc_header_str=recv_buf.substr(4,header_size);//substr从4开始读读取header_size个字节的数据
    mprpc::RpcHeader rpcHeader;
    std::string service_name;//用于存储反序列化成功的服务名字
    std::string method_name;//用于存储反序列化成功的服务方法
    uint32_t args_size;//用于存储反序列化成功的参数个数

    std::cout<<"======================================"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str"<<rpc_header_str<<std::endl;
    std::cout<<"======================================"<<std::endl;
    if(rpcHeader.ParseFromString(rpc_header_str))//开始反序列化
    {

        service_name=rpcHeader.service_name();
        method_name=rpcHeader.method_name();
        args_size=rpcHeader.args_size();
    }
    else
    {
        std::cout<<"rpc_header_str: "<<rpc_header_str<<"parse error!"<<std::endl;
        return;
    }


    std::string args_str=recv_buf.substr(4+header_size,args_size);

    std::cout<<"======================================"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str"<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"======================================"<<std::endl;

    //获取service对象和method对象
    auto it =m_serviceMap.find(service_name);
    if(it==m_serviceMap.end())
    {
        //如果方法不存在
        std::cout<<service_name<<"is not exist!"<<std::endl;
        return;
    }

    auto mit=it->second.m_methodMap.find(method_name);
    if(mit==it->second.m_methodMap.end())
    {
        //如果服务提供的方法不存在
        std::cout<<service_name<<":"<<method_name<<"is not exists!"<<std::endl;
    }
    google::protobuf::Service *service=it->second.m_service;//获取service对象，对应Userservice
    const google::protobuf::MethodDescriptor *method=mit->second;//获取method对象，对应Login方法

    //生成rpc方法调用的请求request和相应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();//生成一个新对象
    if(!request->ParseFromString(args_str))
    {
        std::cout<<"request parse error,content:"<<args_str<<std::endl;
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();//生成一个新对象

    google::protobuf::Closure *done=google::protobuf::NewCallback
    <RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
    (this,&RpcProvider::SendRpcResponse,conn,response);
 

    //相当于UserService调用了Login方法,这里的service是用google::protobuf::Service来指向，UserServiceRpc重写了CallMethod方法，
    //UserService又继承自UserServiceRpc，相当于这里调用的是UserService的CallMethod方法，实现对UserService这个服务的所有方法进行遍历
    service->CallMethod(method,nullptr,request,response,done);
}


void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        std::cout<<"Serialize response error!"<<std::endl;
    }
    conn->shutdown();
}