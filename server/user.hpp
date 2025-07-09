#pragma once
#include<iostream>
#include<string>
#include<vector>
#include "../userdata.hpp"
#include "../Redis.hpp"

class user{
private:
redisContext * redis;
std::string name;
std::string email;
std::string pwd;
std::string stat;
std::vector<std::string> frd;//存储好友的uid
std::vector<std::string> group; //存储群聊的gid

public:

    user():redis(connectRedis()){
    }
    //根据json字符串注册用户
    bool newuser(std::string);
    //获取一个未使用的uid
    std::string newuid(void);
    //根据用户名返回用户id，若用户不存在则返回“norepeat”
    std::string Getuid(const char *);
    //判断电子邮箱是否已存在，返回true已存在，返回false不存在
    bool RepeatEmail(const char *);
    //根据用户uid拿到用户名称
    std::string GetUserName(std::string );
    //根据用户uid拿到用户邮箱
    std::string GetUserEmail(std::string );
    //根据用户uid拿到用户密码（哈希值）
    std::string GetUserPwd(std::string );
    //根据用户uid拿到用户状态
    std::string GetUserStat(std::string );
};

bool user::newuser(std::string str){
    //拿到用户数据
    int i = 0;
    while(str[i] != ':') i++;
    std::string s = str.c_str() + i + 1;
    printf("拿到json:%s\n",s.c_str());
    UserData ud = UserData::fromJson(s);  
    //生成用户uid
    std::string nuid = newuid();
    //写入用户名->uid的键值对
    redisReply* reply = (redisReply*)redisCommand(redis, "SET username:%s %s", ud.name.c_str(), nuid.c_str());
    freeReplyObject(reply);
    //写入json字符串
    reply = (redisReply*)redisCommand(redis, "SET user:%s %s", nuid.c_str(), s.c_str());
    freeReplyObject(reply);
    //将邮箱写入集合
    reply = (redisReply*)redisCommand(redis, "SADD email %s", ud.email.c_str());
    freeReplyObject(reply);
    return true;
}

std::string user::newuid(){
    int i;
    redisReply* reply = NULL;
    for(i = 1001; i < 1000000; i++){
        reply = (redisReply*)redisCommand(redis, "GET user:%d", i);
        if(reply->type == REDIS_REPLY_NIL) break;
        else freeReplyObject(reply);
    }
    char buf[20];
    sprintf(buf, "%d", i);
    if(reply != NULL)
        freeReplyObject(reply);
    return buf;
}

bool user::RepeatEmail(const char * buf){
    int i = 0;
    while(buf[i] != ':') i++;
    std::string s = buf + i + 1;
    printf("拿到email:%s\n",s.c_str());
    redisReply* reply = (redisReply*)redisCommand(redis, "SISMEMBER email %s", s.c_str());
    bool ret = false;
    if(reply->integer == 1) ret = true;//存在
    freeReplyObject(reply);
    return ret;
}

//根据用户名返回用户id，若用户不存在则返回“norepeat”
std::string user::Getuid(const char * buf){
    int i = 0;
    while(buf[i] != ':') i++;
    std::string s = buf + i + 1;
    printf("拿到username:%s\n",s.c_str());
    redisReply* reply = (redisReply*)redisCommand(redis, "GET username:%s", s.c_str());
    if(reply->type == REDIS_REPLY_NIL){
        freeReplyObject(reply);
        return "norepeat";
    } else {
        std::string ret = reply->str;
        freeReplyObject(reply);
        return ret;
    }
}
