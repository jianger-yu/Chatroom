#pragma once
#include "../MessageQueue.hpp"

class groupfucs{
private:
    user& u;
    void* clientp;

    char name[512];

public:

    groupfucs(user& arg1, void*p):u(arg1),clientp(p){
    };

    void creategroup();

};

void groupfucs::creategroup(){
    Client * cp = (Client*)clientp;
    Socket* sock = cp->getSocket();
    std::string str;
    system("clear");
    printf("\033[0;32m请输入您要创建的群聊名:\033[0m\n>");

    while(1){
        //int ret = read(STDIN_FILENO,name,sizeof name);
        chu(name);
        int ret = enter(name, 0);
        if(ret == -1) return;
        //检测空格
        int key = 0;
        for(int i = 0; i < ret; i++){
            if(name[i] == ' ') {
                key = 1;
                break;
            }
        }
        if(key == 1){
            key = 0;
            printf("\033[0;31m群聊名仅允许英文字母、数字和汉字，请重新输入。\n\033[0m>");
            continue;
        }
        //判断群聊名合法性
        int tmp = is_valid_username(name);
        if(tmp == -1){
            printf("\033[0;31m群聊名仅允许英文字母、数字和汉字，请重新输入。\n\033[0m>");
            continue;
        }
        if(tmp <= 3) {
            printf("\033[0;31m群聊名至少为4位，请重新输入。\n\033[0m>");
            continue;
        }
        //判断是否有重名
        //询问服务器
        sock->sendMsg("jrgn:"+std::string(name));//judge_repeat_name
        //sock->sendMsg(name);
        str = EchoMsgQueue.wait_and_pop();
        if(strcmp(str.c_str(), "norepeat") != 0){
            printf("\033[0;31m群聊名已存在，请重新输入。\n\033[0m>");
            continue;
        }
        //创建该群聊
        sock->sendMsg("ctgp:"+std::string(name)+":"+u.uid);
        str = EchoMsgQueue.wait_and_pop();
        u.grouplist.insert(str);
        break;
    }
    printf("\033[0;32m创建群聊成功!\n\033[0m");
    printf("\033[0;32m群名:\033[0m%s\n", name);
    printf("\033[0;32m群id:\033[0m%s\n", str.c_str());
    printf("\033[0;32m请按任意键继续...\033[0m");
    charget();
}