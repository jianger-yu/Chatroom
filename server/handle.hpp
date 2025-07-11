#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include "userdata.hpp"
#include "sendrecv.hpp"
#include "../user.hpp"
#include "../client/register/hashpwd.hpp"

class handler{
private:
    std::string str;
    int sockfd;
    userdata u;
    //处理是否重名的请求
    void jrnm();
    //处理邮箱是否注册过的请求
    void jrem();
    //处理用户名和邮箱是否是同一组的请求
    void jrne();
    //处理找回密码的请求
    void fdpd();
    //处理向邮箱集合中增加元素的请求
    void emad();
    //处理注册的请求
    void torgst();
    //处理账号密码登录的请求
    void pwlg();
    //处理邮箱验证码登录成功的请求
    void emlg();
    //利用uid处理登录成功的后端数据,返回user的json字符串
    std::string lgok(std::string);
    //利用用户名处理用户下线的请求
    void unlg(std::string);
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
    else if(str[0] == 'j' && str[1] == 'r' && str[2] == 'n' && str[3] == 'e') jrne();
    else if(str[0] == 'p' && str[1] == 'w' && str[2] == 'l' && str[3] == 'g') pwlg();
    else if(str[0] == 'f' && str[1] == 'd' && str[2] == 'p' && str[3] == 'd') fdpd();
    else if(str[0] == 'e' && str[1] == 'm' && str[2] == 'l' && str[3] == 'g') emlg();
    else if(str[0] == 'u' && str[1] == 'n' && str[2] == 'l' && str[3] == 'g') unlg(str);
}

void handler::jrnm(){
    //根据用户名发送用户id，若用户不存在则返回“norepeat”
    int i = 0;
    while(str[i] != ':') i++;
    std::string buf = str.c_str() + i + 1;
    sendMsg(u.Getuid(buf.c_str()), sockfd);
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

void handler::jrne(){
    std::string name,email;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        name.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) email.push_back(str[t]);
    //获得uid
    std::string usuid = u.Getuid(name.c_str());
    //获得用户数据
    user U = u.GetUesr(usuid);

    if(email == U.email) sendMsg("right", sockfd);
    else sendMsg("false", sockfd);
}

void handler::pwlg(){
    std::string name,pwd;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        name.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) pwd.push_back(str[t]);
    printf("用户名:%s\n", name.c_str());
    printf("密码:%s\n", pwd.c_str());
    //获得uid
    std::string usuid = u.Getuid(name.c_str());
    //若该用户不存在
    if(usuid == "norepeat"){
        sendMsg("pwdfalse", sockfd);
        return;
    }
    //获得用户数据
    user U = u.GetUesr(usuid);
    PasswordHasher hspwd;
    if(hspwd.verifyPassword(pwd.c_str(), U.pwd.c_str())){
        //用户名密码正确
        //发送json序列
        std::string js = lgok(usuid);
        sendMsg( js , sockfd);
    } else{
        //用户名密码错误
        sendMsg("pwdfalse", sockfd);
    }
}

void handler::fdpd(){
    std::string email,pwd;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        email.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) pwd.push_back(str[t]);
    //拿到用户信息
    std::string id = u.EmailGetuid(email.c_str());
    if(id == "norepeat") printf("\033[0;31mIn fuc fdpd ,EmainGetuid return norepeat\033[0m\n");
    user us = u.GetUesr(id);
    //删除用户的user:uid->json信息
    u.DELUesr(id);
    //修改用户密码
    us.pwd = pwd;
    std::string js = us.toJson();
    u.setutoj(id, js);
    sendMsg("right", sockfd);
}

void handler::emlg(){
    int i = 0;
    while(str[i] != ':') i++;
    std::string buf = str.c_str() + i + 1;
    std::string argu = u.EmailGetuid(buf.c_str());
    if(argu == "norepeat"){
        printf("emlg argu is norepeat!\n");
        sendMsg("false", sockfd);
        return;
    }
    //发送json序列
    sendMsg(lgok(argu), sockfd);
}

std::string handler::lgok(std::string uid){
    //拿到用户信息
    user ud = u.GetUesr(uid);
    //切换在线状态
    ud.stat = "online";
    //获取json序列
    std::string js = ud.toJson();
    u.setutoj(uid, js);
    //将套接字存入map
    uid_to_socket[uid] = sockfd;
    socket_to_uid[sockfd] = uid;
    return js;
}

void handler::unlg(std::string buf){
    //拿到名字字符串
    int i = 0;
    while(buf[i] != ':') i++;
    std::string name = buf.c_str() + i + 1;
    //拿到uid
    std::string uid = u.Getuid(name.c_str());
    if(uid == "norepeat") return;
    //拿到用户信息
    user ud = u.GetUesr(uid);
    ud.stat = "offline";
    //将该用户在map存的映射清空
    uid_to_socket.erase(uid);
    socket_to_uid.erase(sockfd);
    //获取json字符串
    std::string js = ud.toJson();
    u.setutoj(uid, js);
}
