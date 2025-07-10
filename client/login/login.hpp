#pragma once
#include<iostream>
#include <fcntl.h>
#include "../client.h"
#include "../../user.hpp"
#include "../register/SendEmail.hpp"
#include <unistd.h>
#include <cstring>

class login{
private:
    char name[256];
    char pwd[256];
    char email[256];

public:

    void lgin(void*p);

};

void login::lgin(void*p){
    Client* client = (Client*) p;
    Socket* sock = client->getSocket();

    system("clear");
    printf("\033[0;32m已选择登录选项(按下ESC可返回上一级菜单)\033[0m\n");
    printf("\033[0;32m请选择您的登录方式:\n\033[0m");

    //输入用户名和密码
    while(1){
        
    }
    

}