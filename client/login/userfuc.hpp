#pragma once

#include "../menu.hpp"


class userfuc{
private:
    user u;
    void* clientp;
    std::thread recvThread;

public:
    userfuc(std::string js):u(user::fromJson(js)){
    };

    int mainfuc(void*p);

    int friendfuc(void);
    int groupfuc(void);
    int recordfuc(void);
    int setupfuc(void);
    int reportfuc(void);

};

int userfuc::mainfuc(void * p){
    clientp = p;
    //Socket* sock = client->getSocket();
    char input = 0;
    system("clear");
    fflush(stdout); // 手动刷新标准输出缓冲区
    usermainmenu(u, clientp);
    std::string msg;
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            usermainmenu(u, clientp);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{
            friendfuc();
            flag = true;
            break;
        }
        case '2':{
            groupfuc();
            flag = true;
            break;
        }
        case '3':{
            recordfuc();
            flag = true;
            break;
        }
        case '4':{
            reportfuc();
            flag = true;
            break;
        }
        case '5':{
            if(setupfuc() == -1) return -2;
            flag = true;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::friendfuc(void){
    char input = 0;
    friendfucs f(u, clientp);
    std::string msg;
    system("clear");
    fflush(stdout); // 手动刷新标准输出缓冲区
    friendmenu(u, clientp);
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            friendmenu(u, clientp);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{
            f.addfriend();
            flag = true;
            break;
        }
        case '2':{
            f.delfriend();
            flag = true;
            break;
        }
        case '3':{
            f.listfriend();
            flag = true;
            break;
        }
        case '4':{
            
            break;
        }
        case '5':{
            
            break;
        }
        case 27:{
            return 0;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::groupfuc(void){
    char input = 0;
    std::string msg;
    system("clear");
    fflush(stdout); // 手动刷新标准输出缓冲区
    groupmenu(u, clientp);
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            groupmenu(u, clientp);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{
            
            break;
        }
        case '2':{
            
            break;
        }
        case '3':{
            
            break;
        }
        case '4':{
            
            break;
        }
        case '5':{
            
            break;
        }
        case 27:{
            return 0;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::recordfuc(void){
    char input = 0;
    std::string msg;
    system("clear");
    recordmenu(u, clientp);
    fflush(stdout); // 手动刷新标准输出缓冲区
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            system("clear");
            recordmenu(u, clientp);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{
            
            break;
        }
        case '2':{
            
            break;
        }
        case '3':{
            
            break;
        }
        case '4':{
            
            break;
        }
        case '5':{
            
            break;
        }
        case 27:{
            return 0;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::setupfuc(void){
    setupfucs stf(u, clientp);
    Client * cp = (Client*)clientp;
    Socket* sock = cp->getSocket();
    char input = 0;
    std::string msg;
    system("clear");
    setupmenu(u, clientp);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            setupmenu(u, clientp);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{
            
            break;
        }
        case '2':{
            
            break;
        }
        case '3':{
            stf.ViewAccountInformation();
            flag = true;
            break;
        }
        case '4':{
            sock->sendMsg("unlg:"+u.name);
            return -1;
        }
        case 27:{
            return 0;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::reportfuc(void){
    char input = 0;
    std::string msg;
    system("clear");
    fflush(stdout); // 手动刷新标准输出缓冲区
    reportfucs rpf(u, clientp);
    bool ret = rpf.Getrpt();
    reportmenu( u, rpf.rpt, ret);
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            bool ret = rpf.Getrpt();
            reportmenu( u, rpf.rpt, ret);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{
            rpf.friendreport();
            flag = true;
            break;
        }
        case '2':{
            
            break;
        }
        case '3':{
            
            break;
        }
        case '4':{
            
            break;
        }
        case '5':{
            rpf.notice();
            flag = true;
            break;
        }
        case 27:{
            return 0;
        }
        default:continue;
        }
    }
    return 0;
}
