#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include "userdata.hpp"
#include "sendrecv.hpp"
#include "../user.hpp"

class handler{
private:
    std::string str;
    int sockfd;
    userdata u;
    //处理是否重名的请求
    void jrnm();
    //处理邮箱是否注册过的请求
    void jrem();
    //处理向邮箱集合中增加元素的请求
    void emad();
    //处理注册的请求
    void torgst();

public:
    handler(std::string buf, int fd):str(buf),sockfd(fd){
    }

    //处理用户传来的请求
    void handle();
};

//处理用户传来的请求
void handler::handle(void){
    if(str[0] == 'j' && str[1] == 'r' && str[2] == 'n' && str[3] == 'm') jrnm();
    else if(str[0] == 'r' && str[1] == 'g' && str[2] == 's' && str[3] == 't') torgst();
    else if(str[0] == 'j' && str[1] == 'r' && str[2] == 'e' && str[3] == 'm') jrem();
}

void handler::jrnm(){
    //根据用户名发送用户id，若用户不存在则返回“norepeat”
    sendMsg(u.Getuid(str.c_str()), sockfd);
}

void handler::torgst(){
    //发送用户uid或fail
    sendMsg(u.newuser(str), sockfd);
}

void handler::jrem(){
    if(u.RepeatEmail(str.c_str()))
        sendMsg("repeat", sockfd);
    else sendMsg("norepeat", sockfd);
}