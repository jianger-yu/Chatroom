#pragma once
#include<iostream>
#include <fcntl.h>
#include "../../client.h"
#include "../../../user.hpp"
#include <unistd.h>
#include <cstring>
#include "../MessageQueue.hpp"

class reportfucs{
private:
    user u;
    void* clientp;
    int page = 0;

public:
    report rpt;

    reportfucs(user arg1, void*p):u(arg1),clientp(p){
    };
    //处理好友申请页面
    void friendreport();
    //好友申请页面的菜单
    void menu(char);
    //处理好友申请的函数
    void handleapply(char );
    //获取当前全部通知
    bool Getrpt();
};

bool reportfucs::Getrpt(){
    Client * cp = (Client*)clientp;
    Socket* sock = cp->getSocket();
    sock->sendMsg("gtrp:"+u.uid);
    //读取json字符串
    std::string js;
    js = EchoMsgQueue.wait_and_pop();
    if(js == "none") return false;
    rpt = report::fromJson(js);
    return true;
}

void reportfucs::menu(char c){
    if(!Getrpt()) {
        printf("Get rpt error!!\n");
        exit(1);
    }
    if(rpt.friendapply.size() == 0){
        printf("\033[0;32m当前没有未处理的好友申请。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    int cnt = rpt.friendapply.size(), maxpage = cnt / 5, i = 0;
    if(cnt % 5 != 0) maxpage++;
    if(c == '[' && page == 0) ;
    else if(c == '[') page --;
    if(c == ']' && page+1 >= maxpage) ;
    else if(c == ']') page ++;
    printf("\033[0;36m==========================================================\033[0m\n");
    printf("\033[0;32m以下为您未处理的好友申请\033[0m\n");
    for(std::string str : rpt.friendapply){
        if(i >= 5*page && i < 5*(page+1)){
            printf("\033[0;32m[%d] %d、用户\033[0m \033[0;34m%s\033[0m \033[0;32m请求添加您为好友\033[0m\n", i-5*page+1, i + 1, str.c_str());
        }
    }
    printf("                     \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                \033[0;32m[%d/%d]\033[0m\n",page+1,maxpage);

    printf("\033[0;36m==========================================================\033[0m\n");
    printf("\033[0;32m请输入您要处理的申请前的序号:>\033[0m");
}

void reportfucs::handleapply(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    fflush(stdout);
    int i = 0;
    std::string name, sd;
    for(std::string str : rpt.friendapply)
        if((i++ + 1) == 5*page + c -'0')
            name = str;
    if(!name.size()) return;
    printf("\033[0;32m是否同意用户\033[0m \033[0;34m%s\033[0m \033[0;32m的好友申请？（Y/N）\033[0m\n", name.c_str());
    char input = 0;
    sd = "adf(";
    while(1){
        input = charget();
        if(input != 'Y' && input != 'N' && input != 'y' && input != 'n') continue;
        if(input == 'Y' || input == 'y') sd.push_back('y');
        else sd.push_back('n');
        break;
    }
    //xxx:user1.name:user2:uid(用户1加用户2)
    sock->sendMsg(sd+":"+name+":"+u.uid);
    sd = EchoMsgQueue.wait_and_pop();
    if(sd == "right"){
        if(input == 'Y' || input == 'y')
            printf("\033[0;32m添加好友成功！\033[0m\n");
        else 
            printf("\033[0;32m已拒绝该申请！\033[0m\n");
        printf("\033[0;32m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }  else{
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
}


void reportfucs::friendreport(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0');
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            menu('p');
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            handleapply(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case 27:{
            return ;
        }
        default:continue;
        }
    }
    return ;

}