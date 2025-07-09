#pragma once
#include<iostream>
#include <fcntl.h>
#include "../client/client.h"
#include "SendEmail.hpp"
#include <unistd.h>
#include <cstring>


class Register{
public:
    void rgst(void * p);

private:

    char name[256];
    char pwd[256];
    char email[256];
    
    std::string pmitchar = "~!@#$%^&*()-=[];',./_+";
};


//注册功能逻辑函数
void Register::rgst(void * p){
    Client* client = (Client*) p;
    printf("\033[0;32m\n已选择注册选项(按下ESC可返回上一级菜单)\033[0m\n");
    printf("\033[0;32m请输入您要注册的用户名(仅允许英文字母和数字的组合)\n\033[0m>");

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
            if(!((name[i] >= 'a' && name[i] <= 'z')||(name[i] >= 'A' && name[i] <= 'Z')||(name[i] >= '0' && name[i] <= '9'))){
                printf("\033[0;31m用户名仅允许英文字母和数字，请重新输入。\n\033[0m>");
                key = 1;
                break;
            }
        }
        if(key) continue;
        //判断是否有重名
        //询问服务器
        Socket* sock = client->getSocket();
        sock->sendMsg("jrnm:"+std::string(name));//judge_repeat_name
        //sock->sendMsg(name);
        std::string str;
        sock->recvMsg(str);
        if(strcmp(str.c_str(), "repeat") == 0){
            printf("\033[0;31m用户名已存在，请重新输入。\n\033[0m>");
            continue;
        }
        break;
    }while(1);


    //输入密码
    printf("\033[0;32m请输入您要注册的密码\n\033[0m>");
    do{
        chu(pwd);
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
        break;
    } while(1);
    
    //再次输入密码
    printf("\033[0;32m请再次输入您的密码\n\033[0m>");
    char pwd2[256];
    do{
        chu(pwd2);
        int ret = enter(pwd2, 1);
        if(ret == -1) return;
        if(strcmp(pwd,pwd2) != 0){
            printf("\033[0;31m输入与第一次不符，请重新输入。\n\033[0m>");
            continue;
        }
        break;
    } while(1);

    //哈希密码,加密数据
    PasswordHasher hs;
    hs.hashPassword(pwd);

    //输入电子邮箱
    EmailSender emsend;
    printf("\033[0;32m请输入您要绑定的电子邮箱\n\033[0m>");
    do{
        chu(email);
        int ret = enter(email, 0);
        if(ret == -1) return;
        printf("\033[0;32m正在为您发送验证码...\033[0m");
        fflush(stdout); // 手动刷新标准输出缓冲区
        if(!emsend.send(email)){
            printf("\033[0;31m该电子邮箱无效，请检查并重新输入。\n\033[0m>");
            continue;
        }
        break;
    } while(1);
    
    //输入验证码
    char yan[100];
    printf("\033[0;32m请输入验证码\n\033[0m>");
    do{
        chu(yan);
        int ret = enter(yan, 0);
        if(ret == -1) return;
        if(strcmp(yan, emsend.code) != 0){
            printf("\033[0;31m验证码错误，请检查并重新输入。\n\033[0m>");
            continue;
        }
        break;
    } while(1);

    //注册成功，发给服务器，写入数据库
    printf("user:%s\n",name);
    printf("pwd:%s\n",pwd2);
    printf("hashpwd:%s\n",pwd);
    printf("(pwd,hashpwd):%d\n",hs.verifyPassword(pwd2, pwd));
    printf("email:%s\n",email);
    sleep(1000);
}