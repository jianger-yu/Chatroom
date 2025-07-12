#pragma once
#include "allfucs.h"

class reportfucs{
private:
    user u;
    void* clientp;
    std::vector<std::string> friendapply;  //好友申请消息表    redis存friend:apply:uid         值为申请人uid
    std::vector<std::string> chatfriend;   //未读好友消息表    redis存chat:friend:uid          值为发消息人uid
    std::vector<std::string> chatgroup;    //未读群聊消息表    redis存chat:group:uid (set)     值为有信息群的组gid
    std::vector<std::string> groupapply;   //未读群聊邀请表    redis存gourp:applt:uid          值为发来邀请群的gid
    std::vector<std::string> notice;       //各种事务通知表    redis存notice:uid               值为notice:id

public:

    reportfucs(user arg1, void*p):u(arg1),clientp(p){
    };

};