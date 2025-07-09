#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include"user.hpp"
#include "sendrecv.hpp"

class handler{
private:
std::string str;
int sockfd;

user u;

void jrnm(void);


public:
handler(std::string buf, int fd):str(buf),sockfd(fd){
}
//处理用户传来的请求
void handle();
};

//处理用户传来的请求
void handler::handle(void){
    if(str[0] == 'j' && str[1] == 'r' && str[2] == 'n' && str[3] == 'm') jrnm();

}

void handler::jrnm(){
    //已存在，给客户端发repeat
    if(u.RepeatUserName(str.c_str())){
        sendMsg("repeat", sockfd);
    } else {//不存在，给客户端发norepeat
        sendMsg("norepeat", sockfd);
    }
}

