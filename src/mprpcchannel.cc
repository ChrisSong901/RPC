#include"./include/mprpcchannel.h"
#include"./include/rpcheader.pb.h"
#include"./include/mprpcApplication.h"
#include<string>
#include<errno.h>
#include<unistd.h>

//网络编程部分
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();//获取方法的服务
    std::string service_name = sd->name();
    std::string method_name = method->name();

    uint32_t args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        args_size = args_str.size();

    }else{
        controller->SetFailed("serialize request error!");//保存错误信息
        return;
    }

    RPC::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpcheader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else{
        //std::cout<<"serialize rpc header error!"<<std::endl;优化
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char *)&header_size,4));//header_size
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    std::cout<<"======================================"<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str"<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"======================================"<<std::endl;

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd==-1)
    {
        char errtxt[512]={0};
        sprintf(errtxt,"create socket error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    //读取配置文件rpcservier的信息
    std::string ip = MprpcApplication::GetInstance().getconfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().getconfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if(-1==connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
         close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"connect socket error! errno: %d",errno);

        return;
    }


    if(-1==send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0))
    {
        // std::cout<<"send error!errno: "<<errno<<std::endl;
        // close(clientfd);
        // return;//结束本次rpc调用
        close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"send socket error! errno: %d",errno);
        //controller->SetFailed(errtxt);
        return;
    }    
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if(-1==(recv_size=recv(clientfd,recv_buf,1024,0)))
    {
        // std::cout<<"recv error!errno: "<<errno<<std::endl;
        // close(clientfd);
        // return;
        close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"recv socket error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }

    std::cout << recv_buf << std::endl;
    std::string response_str(recv_buf, 0, recv_size);
    if(!response->ParsePartialFromArray(recv_buf,recv_size))
    {
        // std::cout<<"parse error! response_str:"<<response_str<<std::endl;
        // close(clientfd);
        // return;
        close(clientfd);
        char errtxt[512]={0};
        sprintf(errtxt,"recv socket error! errno: %s",recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(clientfd);
}