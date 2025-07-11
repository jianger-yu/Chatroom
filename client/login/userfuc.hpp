#pragma once
#include<iostream>
#include <fcntl.h>
#include "../client.h"
#include "../../user.hpp"
#include "../register/SendEmail.hpp"
#include <unistd.h>
#include <cstring>

class userfuc{
private:
    user u;
    void* clientp;

public:
    userfuc(){
    };

    int mainfuc(void*p);

    int friendfuc(void);
    int groupfuc(void);
    int recordfuc(void);
    int setupfuc(void);
};

int userfuc::mainfuc(void * p){
    clientp = p;
    //Socket* sock = client->getSocket();

    char input = 0;
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        usermainmenu();
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
            if(setupfuc() == -1) return -2;
        }
        default:continue;
        }
    }
    return 0;
}

int userfuc::friendfuc(void){
    char input = 0;
    while(1){
        system("clear");
        fflush(stdout); // 手动刷新标准输出缓冲区
        friendmenu();
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