#pragma once
#include "../MessageQueue.hpp"

class friendfucs{
private:
    user u;
    void* clientp;

public:

    friendfucs(user arg1, void*p):u(arg1),clientp(p){
    };
    //添加好友
    void addfriend();
    //删除好友
    void delfriend();
    //查看好友列表
    void listfriend();
    //私聊
    void chatfriend();
    //屏蔽/解除屏蔽好友
    void shieldfriend();
};

void friendfucs::addfriend(){
    Client* c = (Client*)clientp;
    Socket* sock = c->getSocket();
    system("clear");
    printf("\033[0;32m已选择添加好友功能(按ESC可返回上一级菜单)\033[0m\n");
    std::string red;
    char buf[520];
    char uid[256];
    printf("\033[0;32m请输入您要添加好友的uid:\n\033[0m>");
    while(1){
        chu(uid);
        if(enter(uid, 0) == -1) return ; 
        if(strlen(uid) <= 3){
            printf("\033[0;31m该uid无效，请重新输入。\033[0m\n>");
            continue;
        }
        if(uid == u.uid){
            printf("\033[0;31m不能添加自己为好友，请重新输入。\033[0m\n>");
            continue;
        }
        //判断此人是否存在
        sprintf(buf, "jrud:%s", uid);
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "norepeat"){
            printf("\033[0;31m该uid无效，请重新输入。\033[0m\n>");
            continue;
        }
        //判断是否已为好友
        chu(buf);
        sprintf(buf, "jrfd:%s:%s", uid, u.uid.c_str());
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "right"){
            printf("\033[0;31m您与%s用户已为好友，无需添加，请重新输入。\033[0m\n>", uid);
            continue;
        }
        
        chu(buf);
        red.clear();
        printf("\033[0;32m正在发送好友申请...\n\033[0m");
        fflush(stdout); // 手动刷新标准输出缓冲区
        //给服务器处理
        chu(buf);
        sprintf(buf, "adfr:%s:%s",u.uid.c_str(), uid);
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "right") {
            printf("\033[0;32m好友申请发送成功，请按任意键继续...\n\033[0m");
            char rub;
            rub = charget();
            return;
        } else{
            printf("\033[0;31m\n数据异常，请稍后再试。\033[0m\n");
            continue;
        }
    }
    return ;

}
