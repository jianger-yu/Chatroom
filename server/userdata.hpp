#pragma once
#include<iostream>
#include<string>
#include<vector>
#include "../user.hpp"
#include "../group.hpp"
#include "../Redis.hpp"
#include "report.hpp"
#include "../message.hpp"
#include "sendrecv.hpp"

class userdata{
private:
redisContext * redis;
std::string uid;
std::string name;
std::string email;
std::string pwd;
std::string stat;
std::vector<std::string> friendlist;//存储好友的uid
std::vector<std::string> grouplist; //存储群聊的gid
std::vector<std::string> shieldlist; //存储黑名单的uid

public:

    userdata():redis(connectRedis()){
    }
    //根据json字符串注册用户,返回uid或fail
    std::string newuser(std::string);
    std::string newgroup(std::string&);

    //根据uid和json字符串存储report
    bool svreport(std::string uid, std::string js);
    //获取一个未使用的uid
    std::string newuid(void);
    std::string newgid(void);

    //根据用户名返回用户id，若用户不存在则返回“norepeat”
    std::string Getuid(const char *);
    std::string Getgid(const char * buf);

    //根据用户uid返回用户结构体
    user GetUesr(std::string);
    //返回group的json字符串，若不存在，返回norepeat
    std::string GetGroup(std::string);

    //根据email返回用户id，若用户不存在则返回“norepeat”
    std::string EmailGetuid(const char *);
    //判断电子邮箱是否已存在，返回true已存在，返回false不存在
    bool RepeatEmail(const char *);
    

    //根据用户uid删除用户结构体
    bool DELUesr(std::string);
    //仅存入user:uid -> json的键值对
    bool setutoj(std::string, std::string);
    //仅存入group:gid -> json的键值对
    bool setgtoj(std::string, std::string);

    //将uid1放在uid2的好友申请表内（uid1加uid2）
    bool AddFrd(std::string uid1, std::string uid2);
    //判断传入的uid是否存在，存在返回true，不存在返回false
    bool jguid(std::string uid);
    //根据uid获取用户的report(json字符串)，成功返回json，失败返回none
    std::string u_report(std::string uid);
    //根据uid1、uid2、消息条数、拉取顺序获取消息列表，fg为0拉最新消息，为1拉最早消息
    std::vector<std::string> lrange(std::string uid1, std::string uid2, int cnt, int fg);
    std::vector<std::string> glrange(std::string gid, int cnt, int fg);
    //根据uid1、uid2获取消息总量
    int llen(std::string uid1, std::string uid2);
    int gllen(std::string gid);
    //根据聊天的json字符串存聊天记录
    void savechat(std::string js);
    void savechat2(std::string js);
    void savegchat(std::string js);
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
    ud.uid = nuid;
    //写入用户名->uid的键值对
    redisReply* reply = (redisReply*)redisCommand(redis, "SET username:%s %s", ud.name.c_str(), nuid.c_str());
    freeReplyObject(reply);
    //写入邮箱->uid的键值对
    reply = (redisReply*)redisCommand(redis, "SET email:%s %s", ud.email.c_str(), nuid.c_str());
    freeReplyObject(reply);
    //写入json字符串
    reply = (redisReply*)redisCommand(redis, "SET user:%s %s", nuid.c_str(), ud.toJson().c_str());
    freeReplyObject(reply);
    //将邮箱写入集合
    reply = (redisReply*)redisCommand(redis, "SADD email %s", ud.email.c_str());
    freeReplyObject(reply);
    //生成report数据
    report rpt;
    rpt.total_group_msg = rpt.total_friend_msg = 0;
    svreport(nuid, rpt.toJson());
    return nuid;
}

std::string userdata::newgroup(std::string &str){
    //拿到数据
    std::string gname,uid;
    int i = 0;
    while(str[i] != ':') i++;
    int j = i + 1;
    while(str[j] != ':') {
        gname.push_back(str[j]);
        j++;
    }
    for(int t = j + 1; t < str.size(); t++) uid.push_back(str[t]);
    group gd;
    //生成gid
    std::string ngid = newgid();
    gd.name = gname; 
    gd.gid = ngid;
    gd.owner = uid;
    //写入组名->gid的键值对
    redisReply* reply = (redisReply*)redisCommand(redis, "SET groupname:%s %s", gd.name.c_str(), ngid.c_str());
    freeReplyObject(reply);
    //写入json字符串
    reply = (redisReply*)redisCommand(redis, "SET group:%s %s", ngid.c_str(), gd.toJson().c_str());
    freeReplyObject(reply);
    //获取用户结构体，修改数据
    user ud = GetUesr(uid);
    ud.grouplist.insert(ngid);
    setutoj(uid, ud.toJson());
    if(uid_to_socket.count(ud.uid)) sendMsg("user:"+ud.toJson(), uid_to_socket[ud.uid]);
    return ngid;
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

std::string userdata::newgid(){
    int i;
    redisReply* reply = NULL;
    reply = (redisReply*)redisCommand(redis, "INCR newgid");
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

//根据群聊名返回gid，若用户不存在则返回“norepeat”
std::string userdata::Getgid(const char * buf){
    redisReply* reply = (redisReply*)redisCommand(redis, "GET groupname:%s", buf);
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

std::string userdata::GetGroup(std::string buf){
    redisReply* reply = (redisReply*)redisCommand(redis, "GET group:%s", buf.c_str());
    std::string ret = "norepeat";
    if(reply == NULL || reply->type == REDIS_REPLY_NIL || reply->type != REDIS_REPLY_STRING)
        return ret;
    ret = reply->str;
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
bool userdata::setgtoj(std::string ngid, std::string s){
    redisReply*reply = (redisReply*)redisCommand(redis, "SET group:%s %s", ngid.c_str(), s.c_str());
    freeReplyObject(reply);
    return true;
}


bool userdata::AddFrd(std::string name1, std::string uid2){
    std::string js = u_report(uid2);
    report rpt;
    if(js != "none") 
        rpt = report::fromJson(js);
    rpt.friendapply.insert(name1);
    svreport(uid2, rpt.toJson());
    return true;
}

bool userdata::jguid(std::string uid){
    redisReply* reply = (redisReply*)redisCommand(redis, "EXISTS user:%s", uid.c_str());
    bool ret = false;
    if (reply && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1)
        ret = true;
    if (reply) freeReplyObject(reply);
    return ret;
}

std::string userdata::u_report(std::string uid){
    redisReply* reply = (redisReply*)redisCommand(redis, "GET report:%s",uid.c_str());
    printf("\033[0;31mGET uid:%s\033[0m\n", uid.c_str());

    if(reply == NULL || reply->type == REDIS_REPLY_NIL || reply->type != REDIS_REPLY_STRING){
        freeReplyObject(reply);
        printf("\033[0;31mreturn none, reply->str:%s\033[0m\n", reply->str);
        return "none";
    }
    std::string ret = reply->str;
    freeReplyObject(reply);
    return ret;
}

bool userdata::svreport(std::string uid, std::string js){
    //写入json字符串
    redisReply* reply = (redisReply*)redisCommand(redis, "SET report:%s %s", uid.c_str(), js.c_str());
    freeReplyObject(reply);
    return true;
}
int userdata::llen(std::string uid1, std::string uid2){
    // 先获取list长度
    std::string chat_key = "chat:" + uid1 + ":" + uid2;
    redisReply* reply_len = (redisReply*)redisCommand(redis, "LLEN %s", chat_key.c_str());
    if (!reply_len || reply_len->type != REDIS_REPLY_INTEGER) {
        if(reply_len) freeReplyObject(reply_len);
        return 0; // 失败返回空
    }
    int len = (int)reply_len->integer;
    freeReplyObject(reply_len);
    return len;
}

int userdata::gllen(std::string gid){
    // 先获取list长度
    std::string chat_key = "gchat:" + gid;
    redisReply* reply_len = (redisReply*)redisCommand(redis, "LLEN %s", chat_key.c_str());
    if (!reply_len || reply_len->type != REDIS_REPLY_INTEGER) {
        if(reply_len) freeReplyObject(reply_len);
        return 0; // 失败返回空
    }
    int len = (int)reply_len->integer;
    freeReplyObject(reply_len);
    return len;
}



std::vector<std::string> userdata::lrange(std::string uid1, std::string uid2, int start, int stop){
    std::vector<std::string> result;
    std::string chat_key = "chat:" + uid1 + ":" + uid2;
    int len = 0;
    if (len == llen(uid1, uid2)) return result; // 无消息直接返回

    redisReply* reply = (redisReply*)redisCommand(redis, "LRANGE %s %d %d", chat_key.c_str(), start, stop);
    if (!reply || reply->type != REDIS_REPLY_ARRAY) {
        if(reply) freeReplyObject(reply);
        return result;
    }

    for (int i = 0; i < reply->elements; i++) {
        if(reply->element[i]->type == REDIS_REPLY_STRING)
            result.push_back(reply->element[i]->str);
    }
    freeReplyObject(reply);

    return result;
}

std::vector<std::string> userdata::glrange(std::string gid, int start, int stop){
    std::vector<std::string> result;
    std::string chat_key = "gchat:" + gid;
    int len = 0;
    if (len == gllen(gid)) return result; // 无消息直接返回

    redisReply* reply = (redisReply*)redisCommand(redis, "LRANGE %s %d %d", chat_key.c_str(), start, stop);
    if (!reply || reply->type != REDIS_REPLY_ARRAY) {
        if(reply) freeReplyObject(reply);
        return result;
    }

    for (int i = 0; i < reply->elements; i++) {
        if(reply->element[i]->type == REDIS_REPLY_STRING)
            result.push_back(reply->element[i]->str);
    }
    freeReplyObject(reply);

    return result;
}

void userdata::savechat(std::string js){
    message sendm = message::fromJson(js);
    std::string chat_key = "chat:" + sendm.receiver_uid + ":" + sendm.sender_uid;
    redisReply* reply = (redisReply*)redisCommand(redis, "LPUSH %s %s", chat_key.c_str(), js.c_str());
    freeReplyObject(reply);
}


void userdata::savechat2( std::string js){
    message sendm = message::fromJson(js);
    std::string chat_key = "chat:" + sendm.sender_uid + ":" + sendm.receiver_uid;
    redisReply* reply = (redisReply*)redisCommand(redis, "LPUSH %s %s", chat_key.c_str(), js.c_str());
    freeReplyObject(reply);
}

void userdata::savegchat(std::string js){
    message sendm = message::fromJson(js);
    std::string chat_key = "gchat:" + sendm.receiver_uid;
    redisReply* reply = (redisReply*)redisCommand(redis, "LPUSH %s %s", chat_key.c_str(), js.c_str());
    freeReplyObject(reply);
}