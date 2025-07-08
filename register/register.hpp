#pragma once
#include<iostream>
#include <fcntl.h>
#include "../client/client.h"
#include "SendEmail.hpp"
#include <unistd.h>
#include <cstring>


class Register{
public:
    void rgst(Client &client);

private:

    char name[256];
    char pwd[256];
    char email[256];
    
    std::string pmitchar = "~!@#$%^&*()-=[];',./_+";
};


//注册功能逻辑函数
void Register::rgst(Client &client){
    printf("\033[0;32m已选择注册选项(按下ESC可返回上一级菜单)\033[0m\n");
    printf("\033[0;32m请输入您要注册的用户名(仅允许英文字母)\n\033[0m>");

    //输入用户名
    do{
        //int ret = read(STDIN_FILENO,name,sizeof name);
        chu(name);
        int ret = enter(name, 0);
        if(ret == -1) return;
        if(ret<=3) {
            printf("\033[0;31m用户名至少为4位，请重新输入。\n\033[0m>");
            continue;
        }
        int key = 0;
        for(int i = 0; i < ret; i++){
            if(!((name[i] >= 'a' && name[i] <= 'z')||(name[i] >= 'A' && name[i] <= 'Z'))){
                printf("\033[0;31m用户名仅允许英文字母，请重新输入。\n\033[0m>");
                key = 1;
                break;
            }
        }
        if(key) continue;
        //判断是否有重名
        //询问服务器
        Socket* sock = client.getSocket();
        sock->sendMsg("judge repeat name");
        sock->sendMsg(name);
        std::string str;
        sock->recvMsg(str);
        if(strcmp(str.c_str(), "repeat") == 0){
            printf("\033[0;31m用户名已存在，请重新输入。\n\033[0m>");
            continue;
        }
    }while(0);


    //输入密码
    printf("\033[0;32m请输入您要注册的密码(仅允许英文字母)\n\033[0m>");
    do{
        int ret = enter(pwd, 1);
        if(ret == -1) return;
        else if(ret <= 6) {
            printf("\033[0;31m密码至少为7位，请重新输入。\n\033[0m>");
            continue;
        }
        int key = 0;
        for(int i = 0; i < ret; i++){
            if(!((pwd[i] >= 'a' && pwd[i] <= 'z')||(pwd[i] >= 'A' && pwd[i] <= 'Z')||(pwd[i] >= '0' && pwd[i] <= '9'))){
                int j;
                for(j = 0; j < pmitchar.size(); j++){
                    if(pmitchar[j] == pwd[i]) break;
                }
                if(j == pmitchar.size()){
                    key = 1;
                    printf("\033[0;31密码仅能含有数字、字母和大部分键盘可打出的字符，当前密码不合法，请重新输入。\n\033[0m>");
                    break;
                }
            }
        }
        if(key) continue;
    } while(0);
    
    //再次输入密码
    printf("\033[0;32m请再次输入您的密码\n\033[0m>");
    char pwd2[256];
    do{
        int ret = enter(pwd2, 1);
        if(ret == -1) return;
        if(strcmp(pwd,pwd2) != 0){
            printf("\033[0;31m输入与第一次不符，请重新输入。\n\033[0m>");
            continue;
        }
    } while(0);

    //输入电子邮箱
    printf("\033[0;32m请输入您要绑定的电子邮箱\n\033[0m>");
    do{
        int ret = enter(email, 0);
        if(ret == -1) return;
        EmailSender emsend;
        if(!emsend.send(email)){
            printf("\033[0;31m该电子邮箱无效，请检查并重新输入。\n\033[0m>");
            continue;
        }
    } while(0);
    
}