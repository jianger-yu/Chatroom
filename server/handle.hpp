#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include "userdata.hpp"
#include "sendrecv.hpp"
#include "../user.hpp"
#include "../client/register/hashpwd.hpp"
#include "../message.hpp"

class handler{
private:
    std::string str;
    int sockfd;
    userdata u;
    //判断一个uid是否存在
    void jrud();
    //处理是否重名的请求
    void jrnm();
    //处理邮箱是否注册过的请求
    void jrem();
    //处理用户名和邮箱是否是同一组的请求
    void jrne();
    //处理两个uid是否为好友的请求
    void jrfd();
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
    //处理客户端突然断开后的rvlg指令，即让其下线
    void rvlg();
    //处理用户获取report(json字符串)的请求
    void gtrp();
    //处理用户获取 user (json字符串)的请求
    void gtus();
    //根据用户名获取uid的请求
    void gtud();
    //根据uid获取用户名的请求
    void gtnm();

    //处理添加好友的请求
    void adfr();
    //处理同意/拒绝好友的请求
    void adfok();
    //处理已读通知的请求
    void rdnt();
    //处理删除通知的请求
    void rmnt();
    //处理删除好友的请求
    void rmfd();
    //拉取两页聊天记录
    void ctms();
    //处理用户发送消息的请求
    void sdms();
    //处理用户读取聊天记录总条数的请求
    void rdpg();
    //处理用户需要保存json字符串的需求
    void svrp();
    //处理用户需要提供聊天记录的请求
    void ndms();
public:
    handler(std::string buf, int fd):str(buf),sockfd(fd){
    }

    //利用uid处理用户下线的请求
    void uulg(std::string);
    //处理用户传来的请求
    int handle();
};

//处理用户传来的请求
int handler::handle(void){
    if(str[0] == 'j' && str[1] == 'r' && str[2] == 'n' && str[3] == 'm') jrnm();
    else if(str[0] == 'r' && str[1] == 'g' && str[2] == 's' && str[3] == 't') torgst();
    else if(str[0] == 'j' && str[1] == 'r' && str[2] == 'u' && str[3] == 'd') jrud();
    else if(str[0] == 'j' && str[1] == 'r' && str[2] == 'e' && str[3] == 'm') jrem();
    else if(str[0] == 'j' && str[1] == 'r' && str[2] == 'n' && str[3] == 'e') jrne();
    else if(str[0] == 'j' && str[1] == 'r' && str[2] == 'f' && str[3] == 'd') jrfd();
    else if(str[0] == 'p' && str[1] == 'w' && str[2] == 'l' && str[3] == 'g') pwlg();
    else if(str[0] == 'f' && str[1] == 'd' && str[2] == 'p' && str[3] == 'd') fdpd();
    else if(str[0] == 'e' && str[1] == 'm' && str[2] == 'l' && str[3] == 'g') emlg();
    else if(str[0] == 'u' && str[1] == 'n' && str[2] == 'l' && str[3] == 'g') unlg(str);
    else if(str[0] == 'u' && str[1] == 'u' && str[2] == 'l' && str[3] == 'g') uulg(str);
    else if(str[0] == 'r' && str[1] == 'v' && str[2] == 'l' && str[3] == 'g') {
        rvlg();
        return 1;
    }
    else if(str[0] == 'a' && str[1] == 'd' && str[2] == 'f' && str[3] == 'r') adfr();
    else if(str[0] == 'a' && str[1] == 'd' && str[2] == 'f' && str[3] == '(') adfok();
    else if(str[0] == 'g' && str[1] == 't' && str[2] == 'r' && str[3] == 'p') gtrp();
    else if(str[0] == 'g' && str[1] == 't' && str[2] == 'u' && str[3] == 's') gtus();
    else if(str[0] == 'g' && str[1] == 't' && str[2] == 'u' && str[3] == 'd') gtud();
    else if(str[0] == 'g' && str[1] == 't' && str[2] == 'n' && str[3] == 'm') gtnm();
    else if(str[0] == 'r' && str[1] == 'd' && str[2] == 'n' && str[3] == 't') rdnt();
    else if(str[0] == 'r' && str[1] == 'm' && str[2] == 'n' && str[3] == 't') rmnt();
    else if(str[0] == 'r' && str[1] == 'm' && str[2] == 'f' && str[3] == 'd') rmfd();
    else if(str[0] == 'c' && str[1] == 't' && str[2] == 'm' && str[3] == 's') ctms();
    else if(str[0] == 's' && str[1] == 'd' && str[2] == 'm' && str[3] == 's') sdms();
    else if(str[0] == 'r' && str[1] == 'd' && str[2] == 'p' && str[3] == 'g') rdpg();
    else if(str[0] == 's' && str[1] == 'v' && str[2] == 'r' && str[3] == 'p') svrp();
    else if(str[0] == 'n' && str[1] == 'd' && str[2] == 'm' && str[3] == 's') ndms();

    return 0;
}

void handler::jrud(){
    int i = 0;
    while(str[i] != ':') i++;
    std::string uid = str.c_str() + i + 1;
    if(u.jguid(uid)) sendMsg("echo:repeat", sockfd);
    else sendMsg("echo:norepeat", sockfd);
}


void handler::jrnm(){
    //根据用户名发送用户id，若用户不存在则返回“norepeat”
    int i = 0;
    while(str[i] != ':') i++;
    std::string buf = str.c_str() + i + 1;
    sendMsg("echo:"+u.Getuid(buf.c_str()), sockfd);
}

void handler::torgst(){
    //发送用户uid或fail
    sendMsg("echo:"+u.newuser(str), sockfd);
}

void handler::jrem(){
    if(u.RepeatEmail(str.c_str()))
        sendMsg("echo:repeat", sockfd);
    else sendMsg("echo:norepeat", sockfd);
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

    if(email == U.email) sendMsg("echo:right", sockfd);
    else sendMsg("echo:false", sockfd);
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
        sendMsg("echo:pwdfalse", sockfd);
        return;
    }
    //获得用户数据
    user U = u.GetUesr(usuid);
    PasswordHasher hspwd;
    if(hspwd.verifyPassword(pwd.c_str(), U.pwd.c_str())){
        //用户名密码正确
        //发送json序列
        std::string js = lgok(usuid);
        sendMsg("echo:"+ js , sockfd);
    } else{
        //用户名密码错误
        sendMsg("echo:pwdfalse", sockfd);
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
    sendMsg("echo:right", sockfd);
}

void handler::emlg(){
    int i = 0;
    while(str[i] != ':') i++;
    std::string buf = str.c_str() + i + 1;
    std::string argu = u.EmailGetuid(buf.c_str());
    if(argu == "norepeat"){
        printf("emlg argu is norepeat!\n");
        sendMsg("echo:false", sockfd);
        return;
    }
    //发送json序列
    sendMsg("echo:"+lgok(argu), sockfd);
}

std::string handler::lgok(std::string uid){
    //拿到用户信息
    user ud = u.GetUesr(uid);
    if(ud.stat == "online"){
        //顶号
        sendMsg("lgex:"+ uid, uid_to_socket[uid]);
    }
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
    uulg(uid);
}

void handler::uulg(std::string uid){
    //拿到用户信息
    user ud = u.GetUesr(uid);
    if(uid_to_socket.count(uid) && uid_to_socket[uid] != sockfd)//处理顶号问题
        return ;
    ud.stat = "offline";
    //将该用户在map存的映射清空
    uid_to_socket.erase(uid);
    socket_to_uid.erase(sockfd);
    //获取json字符串
    std::string js = ud.toJson();
    u.setutoj(uid, js);
}

void handler::rvlg(){
    //拿到uid字符串
    int i = 0;
    while(str[i] != ':') i++;
    std::string uid = str.c_str() + i + 1;
    if(!uid.size()) return;
    uulg(uid);
}

void handler::adfr(){
    //拿到uid1要加uid2的信息
    std::string uid1,uid2;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid2.push_back(str[t]);
    //若1不在2的屏蔽表中，发送申请
    user ud = u.GetUesr(uid1);
    u.AddFrd(ud.name, uid2);
    sendMsg("echo:right",sockfd);
    if (uid_to_socket.count(uid2)) {
        sendMsg("rept:"+uid1, uid_to_socket[uid2]);
    }
}

void handler::gtrp(){
    //拿到uid字符串
    int i = 0;
    while(str[i] != ':') i++;
    std::string uid = str.c_str() + i + 1;
    //拿到json/none
    std::string js = u.u_report(uid);
    sendMsg("echo:"+js, sockfd);
}

void handler::gtus(){
    //拿到uid字符串
    int i = 0;
    while(str[i] != ':') i++;
    std::string uid = str.c_str() + i + 1;
    //拿到json/none
    user us = u.GetUesr(uid);
    sendMsg("echo:"+us.toJson(), sockfd);
}


void handler::jrfd(){
    std::string uid1,uid2;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid2.push_back(str[t]);
    user ud1 = u.GetUesr(uid1);
    if(ud1.friendlist.count(uid2)) sendMsg("echo:right", sockfd);
    else sendMsg("echo:false", sockfd);
}



void handler::adfok(){
    int i = 0;
    while(str[i] != '(') i++;
    //获取同意/拒绝标志
    char flag = str[i+1];
    //获取用户名1和uid2
    std::string name1,uid2,uid1,js;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        name1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid2.push_back(str[t]);
    //清理ud2的report，并保存
    js = u.u_report(uid2);
    if(js == "none"){
        printf("In fuc adfok uid2:%s report return none\n", uid2.c_str());
        sendMsg("echo:false", sockfd);
    }
    report rpt2 = report::fromJson(js);
    rpt2.friendapply.erase(name1);
    u.svreport(uid2, rpt2.toJson());
    //拿到uid1和ud2
    uid1 = u.Getuid(name1.c_str());
    user ud1 = u.GetUesr(uid1), ud2 = u.GetUesr(uid2);
    if(flag == 'y'){
        //获取u1和u2的用户数据
        ud1.friendlist.insert(uid2);
        ud2.friendlist.insert(uid1);
        //保存ud1、ud2
        u.setutoj(uid1, ud1.toJson());
        u.setutoj(uid2, ud2.toJson());        
    }
    sendMsg("echo:right", sockfd);
    //获取u1的report
    js = u.u_report(uid1);
    if(js == "none"){
        printf("In fuc adfok uid1:%s report return none\n", uid1.c_str());
        sendMsg("echo:false", sockfd);
        return;
    }
    report rpt1 = report::fromJson(js);
    //若用户2对用户1有好友申请，清除
    if(rpt1.friendapply.count(ud2.name)) rpt1.friendapply.erase(ud2.name);
    //给u1发成功添加好友的notice
    if(flag == 'y')
        rpt1.notice.push_back("adfy(n)" + ud2.name);
    else    
        rpt1.notice.push_back("adfn(n)" + ud2.name);
    //保存rpt1
    u.svreport(uid1, rpt1.toJson());
    //若uid1在线，发通知
    if(uid_to_socket.count(uid1)) {
        sendMsg("rept:"+uid2, uid_to_socket[uid1]);
        sendMsg("user:"+ud1.toJson(), uid_to_socket[uid1]);
    }

}


void handler::rdnt(){
    std::string uid,notice;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) notice.push_back(str[t]);
    std::string js = u.u_report(uid);
    if(js == "none"){
        printf("In fuc adfok uid1:%s report return none\n", uid.c_str());
        sendMsg("echo:false", sockfd);
        return;
    }
    report rpt = report::fromJson(js);
    for(i = 0; i < rpt.notice.size();i++)
        if(rpt.notice[i] == notice){
            rpt.notice[i][5] = 'y';
            break;
        }
    u.svreport(uid, rpt.toJson());
    sendMsg("echo:right", sockfd);

}


void handler::rmnt(){
    std::string uid,notice;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) notice.push_back(str[t]);
    std::string js = u.u_report(uid);
    if(js == "none"){
        printf("In fuc adfok uid1:%s report return none\n", uid.c_str());
        sendMsg("echo:false", sockfd);
        return;
    }
    report rpt = report::fromJson(js);
    for(i = 0; i < rpt.notice.size();i++)
        if(rpt.notice[i] == notice){
            rpt.notice.erase(rpt.notice.begin() + i);
            break;
        }
    u.svreport(uid, rpt.toJson());
    sendMsg("echo:right", sockfd);
}

//uid1删uid2
void handler::rmfd(){
    //拿到数据
    std::string uid1,uid2;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid2.push_back(str[t]);
    //获取user
    user ud1 = u.GetUesr(uid1), ud2 = u.GetUesr(uid2);
    ud1.friendlist.erase(uid2);
    ud2.friendlist.erase(uid1);
    std::string js = ud2.toJson();
    u.setutoj(uid1, ud1.toJson());
    u.setutoj(uid2, js);
    if(uid_to_socket.count(uid2)) sendMsg("user:"+ js, uid_to_socket[uid2]);
    sendMsg("echo:right", sockfd);
}


void handler::gtud(){
    int i = 0;
    while(str[i] != ':') i++;
    std::string name = str.c_str() + i + 1;
    std::string sd = u.Getuid(name.c_str());
    if(sd == "norepeat") sendMsg("echo:false", sockfd);
    else sendMsg("echo:"+sd, sockfd);
}

void handler::gtnm(){
    int i = 0;
    while(str[i] != ':') i++;
    std::string uid = str.c_str() + i + 1;
    user ud = u.GetUesr(uid);
    sendMsg("echo:"+ud.name, sockfd);
}

void handler::ctms(){
    //拿到数据
    std::string uid1,uid2;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid2.push_back(str[t]);
    messages msg(u.lrange(uid1, uid2, 0, 13));
    //发给用户
    sendMsg("echo:"+ msg.toJson(), sockfd);
}

//处理用户发送消息的请求
//"sdms:"+message
void handler::sdms(){
    std::string msg;
    int i = 0, j;
    while(str[i] != ':') i ++;
    msg = str.c_str() + i + 1;
    //存入数据库
    u.savechat(msg);
    message sendm = message::fromJson(msg);
    //编辑接受者的通知
    report rpt = report::fromJson(u.u_report(sendm.receiver_uid));
    int cnt = rpt.chatfriend[sendm.sender_uid];
    rpt.chatfriend[sendm.sender_uid] = cnt + 1;
    rpt.total_friend_msg++;
    u.svreport(sendm.receiver_uid, rpt.toJson());
    //若接受者在线，给接受者发通知：有新消息
    if(uid_to_socket.count(sendm.receiver_uid)) sendMsg("rept:"+sendm.sender_uid, uid_to_socket[sendm.receiver_uid]);
    //给接受者发送这条消息
    if(uid_to_socket.count(sendm.receiver_uid)) sendMsg("chat:"+sendm.toJson(), uid_to_socket[sendm.receiver_uid]);

}

void handler::rdpg(){
    //拿到数据
    std::string uid1,uid2;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid2.push_back(str[t]);
    int t = u.llen(uid1, uid2);
    char arr[512];
    sprintf(arr, "echo:%d", t);
    sendMsg(arr, sockfd);
}

void handler::svrp(){
    //拿到数据
    std::string uid1,sd;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) sd.push_back(str[t]);
    u.svreport(uid1, sd);
    sendMsg("echo:right", sockfd);
}

void handler::ndms(){
    //拿到数据
    std::string uid1, uid2, msgcnt;
    int i = 0, t, cnt;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        uid1.push_back(str[j]);
        j++;
    }
    t = j + 1;
    while(str[t] != ':') {
        uid2.push_back(str[t]);
        t++;
    }
    for(int k = t + 1; k < str.size(); k++) msgcnt.push_back(str[k]);
    sscanf(msgcnt.c_str(), "%d", &cnt);
    messages msgs = u.lrange(uid1, uid2, cnt, cnt+6);
    sendMsg("echo:"+ msgs.toJson(), sockfd);
}