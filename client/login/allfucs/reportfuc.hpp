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
    user& u;
    void* clientp;
    int page = 0;

public:
    report rpt;

    reportfucs(user& arg1, void*p):u(arg1),clientp(p){
    };
    //获取当前全部通知
    bool Getrpt();
    

    //处理好友申请页面
    void friendreport();
    //好友申请页面的菜单
    void menu(char c , int fg);
    //处理好友申请的函数
    void handleapply(char );
    //查看事务通知的页面
    void notice();
    //分析公告
    void Analysisnotice(std::string&str, int i);
    //处理公告
    void handlenotice(char );

    //打印新通知
    static void newreport(user&u, void* p){
        reportfucs rpf(u, p);
        bool ret = rpf.Getrpt();
        if(ret){
        if(rpf.rpt.friendapply.size())
        printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpf.rpt.friendapply.size());
            if(rpf.rpt.total_friend_msg)
        printf("           \033[0;34m[new] 有%d个未读的好友消息\033[0m\n", rpf.rpt.total_friend_msg);
            if(rpf.rpt.total_group_msg)
        printf("           \033[0;34m[new] 有%d个未读的群聊消息\033[0m\n", rpf.rpt.total_group_msg);
            if(rpf.rpt.groupapply.size())
        printf("           \033[0;34m[new] 有%ld条未处理的群聊邀请\033[0m\n", rpf.rpt.groupapply.size());
        int cnt = 0;
        for(int i = 0; i < rpf.rpt.notice.size(); i++)
            if(rpf.rpt.notice[i][5] == 'n') cnt++;
        if(cnt)
        printf("           \033[0;34m[new] 有%d条未读的通知\033[0m\n", cnt);
        }
    }
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

void reportfucs::Analysisnotice(std::string &str, int i){
    if(str[0] == 'a' && str[1] == 'd' && str[2] == 'f'){
        std::string result;
        bool yellow = false;
        if(str[3] == 'y') result = "同意";
        else result = "拒绝";
        if(str[5] == 'n') yellow = true;
        str.erase(str.begin(), str.begin() + 7);
        if(i == -1){
            printf("用户 \033[0;34m%s\033[0m %s了您的好友申请\n", str.c_str(), result.c_str());
            return;    
        }
        if(!yellow) 
            printf("[%d] %d、用户 \033[0;34m%s\033[0m %s了您的好友申请\n", i-5*page+1, i + 1, str.c_str(), result.c_str());
        else
            printf("\033[0;33m[%d] %d、用户\033[0m \033[0;34m%s\033[0m \033[0;33m%s了您的好友申请\033[0m\n", i-5*page+1, i + 1, str.c_str(), result.c_str());
    
    }
}


void reportfucs::menu(char c, int fg){
    if(fg == 1 && rpt.friendapply.size() == 0){
        printf("\033[0;32m当前没有未处理的好友申请。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    } else if(fg == 5 && rpt.notice.size() == 0){
        printf("\033[0;32m当前没有事务通知。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    int cnt = 0;
    if(fg == 1)
        cnt = rpt.friendapply.size();
    else if(fg == 5)
        cnt = rpt.notice.size();
    int maxpage = cnt / 5, i = 0;
    if(cnt % 5 != 0) maxpage++;
    if(c == '[' && page == 0) ;
    else if(c == '[') page --;
    if(c == ']' && page+1 >= maxpage) ;
    else if(c == ']') page ++;
    printf("\033[0;36m==========================================================\033[0m\n");
    if(fg == 1)
        printf("\033[0;32m以下为您未处理的好友申请\033[0m\n");
    else if(fg == 5)
        printf("\033[0;32m以下为您收到的通知,黄色为未读部分\033[0m\n");
    if(fg == 1){
        for(std::string str : rpt.friendapply){
            if(i >= 5*page && i < 5*(page+1)){
                printf("\033[0;32m[%d] %d、用户\033[0m \033[0;34m%s\033[0m \033[0;32m请求添加您为好友\033[0m\n", i-5*page+1, i + 1, str.c_str());
            }
            i++;
        }
    } else if (fg == 5){
        for(std::string str : rpt.notice){
            if(i >= 5*page && i < 5*(page+1)){
                Analysisnotice(str, i);
            }
            i++;
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
    std::string name, sd, rev;
    for(std::string str : rpt.friendapply)
        if((i++ + 1) == 5*page + c -'0')
            name = str;
    if(!name.size()) return;
    printf("\033[0;32m是否同意用户\033[0m \033[0;34m%s\033[0m \033[0;32m的好友申请？（Y/N）\033[0m\n", name.c_str());
    char input = 0;
    sd = "adf(";
    while(1){
        input = charget();
        if(input == 27) return;
        if(input != 'Y' && input != 'N' && input != 'y' && input != 'n') continue;
        if(input == 'Y' || input == 'y') sd.push_back('y');
        else sd.push_back('n');
        break;
    }
    //改本地
    sock->sendMsg("gtud:"+name);
    rev = EchoMsgQueue.wait_and_pop();
    if(rev == "false"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
    u.friendlist.insert(rev);
    //xxx:user1.name:user2:uid(用户1加用户2),改数据库
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
    menu('0', 1);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            if(!Getrpt()) {
                printf("\033[0;31m数据异常，请稍后再试\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m");
                input = charget();
                return;
            }
            menu('p', 1);
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
            menu('[', 1);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 1);
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

void reportfucs::handlenotice(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    fflush(stdout);
    //找到对应消息
    int i = 5*page + c - '0' - 1;
    if(i >= rpt.notice.size()) return;
    std::string str = rpt.notice[i], sd;
    //修改rpt
    rpt.notice[i][5] = 'y';
    sock->sendMsg("rdnt:"+u.uid+":"+str);
    sd = EchoMsgQueue.wait_and_pop();
    if(sd == "false"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        return;
    }

    printf("\033[0;32mNOTICE:\n\033[0m");
    Analysisnotice(str, -1);
    printf("\033[0;32m按r可删除这条通知,按ESC可返回上一级页面。\033[0m");
    fflush(stdout);
    while(1){
        char input = 0;
        input = charget();
        if(input == 27) return;
        else if(input == 'r' || input == 'R'){
            rpt.notice.erase(rpt.notice.begin() + i);
            sock->sendMsg("rmnt:"+u.uid+":"+rpt.notice[i]);
            sd = EchoMsgQueue.wait_and_pop();
            if(sd == "false"){
                printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
                return;
            }
            return;
        } else continue;
    }
}

void reportfucs::notice(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0', 5);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            menu('p', 5);
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
            handlenotice(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[', 5);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 5);
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