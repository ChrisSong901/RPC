#include<iostream>
#include<string>

#include"rpcprovider.h"
#include"mprpcapplication.h"
#include"../user.pb.h"

class UserService:public RPC::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd)
    {
        std::cout<<"doing local service:Login"<<std::endl;
        std::cout<<"name:"<<name<<" pwd"<<pwd<<std::endl;
        return true;
    }

    bool Register(uint32_t id,std::string name,std::string pwd)
    {
        std::cout<<"doing Register service:Login"<<std::endl;
        std::cout<<"id:"<<id<<" name:"<<name<<" pwd:"<<pwd<<std::endl;
        return true;
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::RPC::RegisterRequest* request,
                       ::RPC::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t id=request->id();
        std::string name=request->name();
        std::string pwd=request->pwd();

 
        bool ret=Register(id,name,pwd);

 
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(ret);

        done->Run();
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::RPC::LoginRequest* request,
                       ::RPC::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {

        std::string name=request->name();
        std::string pwd=request->pwd();


        bool loginresult=Login(name,pwd);

  
        RPC::ResultCode *Code=response->mutable_reslut();
        Code->set_errcode(1);
        Code->set_errmsg("Login do error!");
        response->set_success(loginresult);
        done->Run();
    }

};

int main(int argc,char **argv)
{

    MprpcApplication::Init(argc,argv);

    RpcProvider provider;
 
    provider.NotifyService(new UserService());
    
    provider.Run();

    return 0;
}