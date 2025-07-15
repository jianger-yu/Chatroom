#pragma once
#include <string>
#include <set>
#include "json.hpp"

using json = nlohmann::json;

struct user {
    std::string uid;
    std::string name;
    std::string email;
    std::string pwd;
    std::string stat;
    std::set<std::string> friendlist;
    std::set<std::string> grouplist;
    std::set<std::string> shieldlist;

    // 将当前对象转为 JSON 字符串
    std::string toJson() const {
        json j;
        j["uid"] = uid;
        j["name"] = name;
        j["email"] = email;
        j["pwd"] = pwd;
        j["stat"] = stat;
        j["friendlist"] = friendlist;
        j["grouplist"] = grouplist;
        j["shieldlist"] = shieldlist;
        return j.dump();
    }

    // 从 JSON 字符串构造对象
    static user fromJson(const std::string& s) {
        json j = json::parse(s);
        user data;
        data.uid = j["uid"];
        data.name = j["name"];
        data.email = j["email"];
        data.pwd = j["pwd"];
        data.stat = j["stat"];
        data.friendlist = j["friendlist"].get<std::set<std::string>>();
        data.grouplist = j["grouplist"].get<std::set<std::string>>();
        data.shieldlist = j["shieldlist"].get<std::set<std::string>>();
        return data;
    }
};
