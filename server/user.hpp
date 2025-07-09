#pragma once
#include<iostream>
#include<string>
#include<vector>
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
    //判断用户名是否已存在，返回true已存在，返回false不存在
    bool RepeatUserName(const char *);
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


//判断用户名是否存在，返回true已存在，返回false不存在
bool user::RepeatUserName(const char * buf){
    int i = 0;
    while(i != ':') i++;
    std::string s = buf + i + 1;
    redisReply* reply = (redisReply*)redisCommand(redis, "GET username:%s", s.c_str());
    if(reply->type == REDIS_REPLY_NIL){
        return false;
    } else {
        return true;
    }
}
