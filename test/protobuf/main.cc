#include"test.pb.h"
#include<iostream>
#include<string>

using namespace rpcpr;

int main()
{
    // LoginResponse rsp;
    // ResultCode *rc=rsp.mutable_result();//使用指针指向rsp的结果
    // rc->set_errcode(1);//置为1
    // rc->set_errmsg("登陆失败");//输出错误信息

    GetFriendListsResponse rsp;
    ResultCode *rc =rsp.mutable_result();
    rc->set_errcode(0);

    User *user1=rsp.add_friend_list();//添加好友
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    std::cout<<rsp.friend_list_size()<<std::endl;
    return 0;
}
