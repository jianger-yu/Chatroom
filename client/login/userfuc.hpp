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
    int filefuc(void);
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
            filefuc();
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
            f.chatfriend();
            flag = true;
            break;
        }
        case '5':{
            f.shieldfriend(1);
            flag = true;
            break;
        }
        case '6':{
            f.shieldfriend(2);
            flag = true;
            break;
        }
        case '7':{
            f.searchfriend();
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

int userfuc::groupfuc(void){
    char input = 0;
    std::string msg;
    system("clear");
    fflush(stdout); // 手动刷新标准输出缓冲区
    groupmenu(u, clientp);
    groupfucs grf(u, clientp);
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
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
            grf.creategroup();
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

int userfuc::filefuc(void){
    char input = 0;
    std::string msg;
    system("clear");
    filefucs(u, clientp);
    filemenu(u, clientp);
    bool flag = false;
    fflush(stdout); // 手动刷新标准输出缓冲区
    while(1){
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            filemenu(u, clientp);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':{//查看与好友的聊天记录
            
            flag = true;
            break;
        }
        case '2':{
            
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
            stf.modpwd();
            flag = true;
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
            system("clear");
            printf("\033[0;31m确定要退出登录？（Y/N）\033[0m\n");
            fflush(stdout);
            char input;
            bool ext = false;
            while(1){
                input = charget();
                if(input == 27) break;
                if(input != 'Y' && input != 'N' && input != 'y' && input != 'n') continue;
                if(input == 'Y' || input == 'y') ext = true;
                break;
            }
            if(ext){
                sock->sendMsg("unlg:"+u.name);
                return -1;
            } else flag = true;
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
            rpf.ctfrdreport();
            flag = true;
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
