#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

struct report {
    std::vector<std::string> friendapply;  //好友申请消息表    redis存friend:apply:uid         值为申请人uid
    std::vector<std::string> chatfriend;   //未读好友消息表    redis存chat:friend:uid          值为发消息人uid
    std::vector<std::string> chatgroup;    //未读群聊消息表    redis存chat:group:uid (set)     值为有信息群的组gid
    std::vector<std::string> groupapply;   //未读群聊邀请表    redis存gourp:applt:uid          值为发来邀请群的gid
    std::vector<std::string> notice;       //各种事务通知表    redis存notice:uid               值为notice:id

    //将当前对象转为JSON字符串
    std::string toJson() const {
        json j;
        j["friendapply"] = friendapply;
        j["chatfriend"] = chatfriend;
        j["chatgroup"] = chatgroup;
        j["groupapply"] = groupapply;
        j["notice"] = notice;
        return j.dump();
    }

    //从JSON字符串构造对象
    static report fromJson(const std::string& s) {
        json j = json::parse(s);
        report data;
        data.friendapply = j["friendapply"].get<std::vector<std::string>>();
        data.chatfriend = j["chatfriend"].get<std::vector<std::string>>();
        data.chatgroup = j["chatgroup"].get<std::vector<std::string>>();
        data.groupapply = j["groupapply"].get<std::vector<std::string>>();
        data.notice = j["notice"].get<std::vector<std::string>>();
        return data;
    }
};
