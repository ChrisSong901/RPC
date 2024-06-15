#include<iostream>
#include<string>
#include"rpcprovider.h"
#include"mprpcapplication.h"
#include"friend.pb.h"
#include"logger.h"
#include<vector>

class FriendService:public RPC::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendList(uint32_t userid)
    {
        std::cout<<" do GetFriendList service!"<<std::endl;
        std::vector<std::string> vec;
        vec.push_back("gao yang");
        vec.push_back("liu hong");
        vec.push_back("wang shuo");
        return vec;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::RPC::GetFriendListRequest* request,
                       ::RPC::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t userid=request->userid();

        std::vector<std::string> friendList=GetFriendList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(std::string &name:friendList)
        {
            std::string *p=response->add_friends();
            *p=name;
        }
        done->Run();
    }
};

int main(int argc,char **argv)
{
    LOG_INFO("first log message!");
    LOG_ERR("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);


    MprpcApplication::Init(argc,argv);


    RpcProvider provider;
    provider.NotifyService(new FriendService());
    
    provider.Run();

    return 0;
}