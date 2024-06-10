#include"../user.pb.h"
#include<iostream>
#include<mprpcconfig.h>
#include"../../src/include/mprpcApplication.h"
#include"../../src/include/rpcprovider.h"
class userservice :public RPC::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd)
    {
        std::cout << "name: " << name << "," << "password:" << pwd << std::endl;
    }
    void Login(::google::protobuf::RpcController* controller,
                       const ::RPC::LoginRequest* request,
                       ::RPC::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool LoginResult = Login(name, pwd);

        RPC::ResultCode* resultcode = response->mutable_result();
        resultcode->set_errcode(0);
        resultcode->set_errmsg("");

        response->set_success(LoginResult);

        done->Run();
    }
};

int main(int argc,char **argv)
{
    MprpcApplication::init(argc, argv);
    RpcProvider rpcprovider;
    rpcprovider.NotifyService(new userservice());
    rpcprovider.Run();

    return 0;
}