#pragma once
#include<iostream>
#include<string>
#include<vector>
#include "../user.hpp"
#include "../Redis.hpp"

class userdata{
private:
redisContext * redis;
std::string name;
std::string email;
std::string pwd;
std::string stat;
std::vector<std::string> frd;//存储好友的uid
std::vector<std::string> group; //存储群聊的gid

public:

    userdata():redis(connectRedis()){
    }
    //根据json字符串注册用户,返回uid或fail
    std::string newuser(std::string);
    //获取一个未使用的uid
    std::string newuid(void);
    //根据用户名返回用户id，若用户不存在则返回“norepeat”
    std::string Getuid(const char *);
    //根据email返回用户id，若用户不存在则返回“norepeat”
    std::string EmailGetuid(const char *);
    //判断电子邮箱是否已存在，返回true已存在，返回false不存在
    bool RepeatEmail(const char *);
    //根据用户uid返回用户结构体
    user GetUesr(std::string);
    //根据用户uid删除用户结构体
    bool DELUesr(std::string);
    //仅存入user:uid -> json的键值对
    bool setutoj(std::string, std::string);
};


std::string userdata::newuser(std::string str){
    //拿到用户数据
    int i = 0;
    while(str[i] != ':') i++;
    std::string s = str.c_str() + i + 1;
    printf("拿到json:%s\n",s.c_str());
    user ud = user::fromJson(s);  
    //生成用户uid
    std::string nuid = newuid();
    //写入用户名->uid的键值对
    redisReply* reply = (redisReply*)redisCommand(redis, "SET username:%s %s", ud.name.c_str(), nuid.c_str());
    freeReplyObject(reply);
    //写入邮箱->uid的键值对
    reply = (redisReply*)redisCommand(redis, "SET email:%s %s", ud.email.c_str(), nuid.c_str());
    freeReplyObject(reply);
    //写入json字符串
    reply = (redisReply*)redisCommand(redis, "SET user:%s %s", nuid.c_str(), s.c_str());
    freeReplyObject(reply);
    //将邮箱写入集合
    reply = (redisReply*)redisCommand(redis, "SADD email %s", ud.email.c_str());
    freeReplyObject(reply);
    return nuid;
}

std::string userdata::newuid(){
    int i;
    redisReply* reply = NULL;
    reply = (redisReply*)redisCommand(redis, "INCR newuid");
    i = reply->integer;
    char buf[20];
    sprintf(buf, "%d", i);
    if(reply != NULL)
        freeReplyObject(reply);
    return buf;
}

bool userdata::RepeatEmail(const char * buf){
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
std::string userdata::Getuid(const char * buf){
    printf("拿到username:%s\n",buf);
    redisReply* reply = (redisReply*)redisCommand(redis, "GET username:%s", buf);
    if(reply->type == REDIS_REPLY_NIL){
        freeReplyObject(reply);
        return "norepeat";
    } else {
        std::string ret = reply->str;
        freeReplyObject(reply);
        return ret;
    }
}

std::string userdata::EmailGetuid(const char * buf){
    printf("拿到email:%s\n",buf);
    redisReply* reply = (redisReply*)redisCommand(redis, "GET email:%s", buf);
    if(reply->type == REDIS_REPLY_NIL){
        freeReplyObject(reply);
        return "norepeat";
    } else {
        std::string ret = reply->str;
        freeReplyObject(reply);
        return ret;
    }
}


user userdata::GetUesr(std::string buf){
    redisReply* reply = (redisReply*)redisCommand(redis, "GET user:%s", buf.c_str());
    //if(reply == NULL || reply->type == REDIS_REPLY_NIL || reply->type != REDIS_REPLY_STRING)
    user ret = user::fromJson(reply->str);
    freeReplyObject(reply);
    return ret;
}

bool userdata::DELUesr(std::string uid){
    redisReply* reply = (redisReply*)redisCommand(redis, "DEL user:%s", uid.c_str());
    bool ret = false;
    if (reply->type == REDIS_REPLY_INTEGER && reply->integer > 0)
        ret = true;
    freeReplyObject(reply);
    return ret;
}

bool userdata::setutoj(std::string nuid, std::string s){
    redisReply*reply = (redisReply*)redisCommand(redis, "SET user:%s %s", nuid.c_str(), s.c_str());
    freeReplyObject(reply);
    return true;
}
