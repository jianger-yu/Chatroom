#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include "json.hpp"

using json = nlohmann::json;

struct report {
    std::set<std::string> friendapply;                 //好友申请消息表    redis存friend:apply:uid         值为申请人名字
    std::map<std::string, int> chatfriend;             //未读好友消息表    redis存chat:friend:uid          值为发消息人名字
    std::unordered_map<std::string, int> chatgroup;    //未读群聊消息表    redis存chat:group:uid (set)     值为有信息群的组名字
    std::set<std::string> groupapply;                  //未读群聊邀请表    redis存gourp:applt:uid          值为发来邀请群的名字
    std::vector<std::string> notice;                   //各种事务通知表    redis存notice:uid               值为notice:id

    int total_friend_msg = 0;                          // 未读好友消息总数
    int total_group_msg = 0;                           // 未读群聊消息总数

    // JSON 序列化
    std::string toJson() const {
        json j;
        j["friendapply"] = friendapply;
        j["chatfriend"] = chatfriend;
        j["chatgroup"] = chatgroup;
        j["groupapply"] = groupapply;
        j["notice"] = notice;
        j["total_friend_msg"] = total_friend_msg;
        j["total_group_msg"] = total_group_msg;
        return j.dump();
    }

    // JSON 反序列化
    static report fromJson(const std::string& s) {
        json j = json::parse(s);
        report data;
        data.friendapply = j["friendapply"].get<std::set<std::string>>();
        data.chatfriend = j["chatfriend"].get<std::map<std::string, int>>();
        data.chatgroup = j["chatgroup"].get<std::unordered_map<std::string, int>>();
        data.groupapply = j["groupapply"].get<std::set<std::string>>();
        data.notice = j["notice"].get<std::vector<std::string>>();
        data.total_friend_msg = j["total_friend_msg"].get<int>();
        data.total_group_msg = j["total_group_msg"].get<int>();
        return data;
    }
};

