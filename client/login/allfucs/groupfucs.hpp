#pragma once
#include "../MessageQueue.hpp"

class groupfucs{
private:
    user& u;
    void* clientp;
    int page = 0;
    char name[512];

public:

    groupfucs(user& arg1, void*p):u(arg1),clientp(p){
    };
    //创建群聊
    void creategroup();
    //打印群聊列表
    void list(char);
    //加群
    void addgroup();
};

void groupfucs::addgroup(){
    Client* c = (Client*)clientp;
    Socket* sock = c->getSocket();
    system("clear");
    printf("\033[0;32m已选择加入群聊功能(按ESC可返回上一级菜单)\033[0m\n");
    std::string red;
    char buf[520];
    char gid[256];
    printf("\033[0;32m请输入您要加入群聊的gid:\n\033[0m>");
    while(1){
        chu(gid);
        if(enter(gid, 0) == -1) return ; 
        if(strlen(gid) <= 3){
            printf("\033[0;31m该gid无效，请重新输入。\033[0m\n>");
            continue;
        }
        //判断是否在群
        if(u.grouplist.count(gid)){
            printf("\033[0;31m已在该群内，不能重复加入。\033[0m\n>");
            continue;
        }
        //判断此群是否存在
        sprintf(buf, "gtgp:%s", gid);
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "norepeat"){
            printf("\033[0;31m该gid无效，请重新输入。\033[0m\n>");
            continue;
        }
        
        chu(buf);
        red.clear();
        printf("\033[0;32m正在发送加群请求...\n\033[0m");
        fflush(stdout); // 手动刷新标准输出缓冲区
        //给服务器处理
        chu(buf);
        sprintf(buf, "adgp:%s:%s",u.uid.c_str(), gid);
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "right") {
            printf("\033[0;32m加群请求发送成功，请按任意键继续...\n\033[0m");
            char rub;
            rub = charget();
            return;
        } else if(red == "ingrp"){
            printf("\033[0;31m已在该群内，不能重复加入。\033[0m\n>");
            continue;
        } else{
            printf("\033[0;31m\n数据异常，请稍后再试。\033[0m\n");
            continue;
        }
    }
    return ;

}

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

void groupfucs::list(char c){
    reportfucs rpf(u, clientp);
    bool ret = rpf.Getrpt();
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    if(!u.grouplist.size()){
        printf("\033[0;32m当前没有加入群组。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    int cnt = 0;
    cnt = u.grouplist.size();
    int maxpage = cnt / 5, i = 0;
    if(cnt % 5 != 0) maxpage++;
    if(c == '[' && page == 0) ;
    else if(c == '[') page --;
    if(c == ']' && page+1 >= maxpage) ;
    else if(c == ']') page ++;
    printf("\033[0;36m==========================================================\033[0m\n");
    reportfucs::newreport(u, clientp);
    printf("\033[0;32m以下为您加入的群组\033[0m\n");
    printf("\033[0;34m%-6s %-14s %-13s %-12s\033[0m\n", "序号", "群名", "GID", "在群中身份");
    for(std::string str : u.grouplist){
        if(i >= 5*page && i < 5*(page+1)){
            sock->sendMsg("gtgp:"+str);
            std::string red = EchoMsgQueue.wait_and_pop();
            if(red == "norepeat") continue;
            group ud = group::fromJson(red);
            std::string name = ud.name, status = "成员";
            const char *color = "\033[0;37m";
            if(ud.owner == u.uid) {
                status = "群主";
                color = "\033[0;31m";
            } else{
                for(std::string str : ud.managelist){
                    if(str == u.uid){
                        status = "管理员";
                        color = "\033[0;32m";
                    }
                }
            }
            printf("\033[0;37m[%d]  %-12s %-14s \033[0m%s%-12s\033[0m",
                     i - 5 * page + 1,
                    name.c_str(), ud.gid.c_str(), color,status.c_str());
            if(rpf.rpt.chatgroup[ud.gid]) printf("   \033[0;31m（%d）\033[0m\n", rpf.rpt.chatgroup[ud.gid]);
            else puts("");
        }
        i++;
    }
    printf("                     \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                \033[0;32m[%d/%d]\033[0m\n",page+1,maxpage);
    printf("\033[0;36m==========================================================\033[0m\n");
}