#pragma once

#include "allfucs/friendfucs.hpp"
#include "allfucs/groupfucs.hpp"
#include "allfucs/recordfuc.hpp"
#include "allfucs/recordfuc.hpp"
#include "allfucs/setupfuc.hpp"

class userfuc{
private:
    user u;
    void* clientp;

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
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        usermainmenu(u);
        input = charget();
        switch(input){
        case '1':{
            friendfuc();
            break;
        }
        case '2':{
            groupfuc();
            break;
        }
        case '3':{
            recordfuc();
            break;
        }
        case '4':{
            reportfuc();
            break;
        }
        case '5':{
            if(setupfuc() == -1) return -2;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::friendfuc(void){
    char input = 0;
    friendfucs f(u, clientp);
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        friendmenu();
        input = charget();
        switch(input){
        case '1':{
            f.addfriend();
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

int userfuc::groupfuc(void){
    char input = 0;
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        groupmenu();
        input = charget();
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
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        recordmenu();
        input = charget();
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
    Client * cp = (Client*)clientp;
    Socket* sock = cp->getSocket();
    char input = 0;
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        setupmenu();
        input = charget();
        switch(input){
        case '1':{
            
            break;
        }
        case '2':{
            
            break;
        }
        case '3':{
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
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        reportmenu();
        input = charget();
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
