#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

struct user {
    std::string uid;
    std::string name;
    std::string email;
    std::string pwd;
    std::string stat;
    std::vector<std::string> friendlist;
    std::vector<std::string> grouplist;
    std::vector<std::string> shieldlist;

    //将当前对象转为JSON字符串
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

    //从JSON字符串构造对象
    static user fromJson(const std::string& s) {
        json j = json::parse(s);
        user data;
        data.uid = j["uid"];
        data.name = j["name"];
        data.email = j["email"];
        data.pwd = j["pwd"];
        data.stat = j["stat"];
        data.friendlist = j["friendlist"].get<std::vector<std::string>>();
        data.grouplist = j["grouplist"].get<std::vector<std::string>>();
        data.shieldlist = j["shieldlist"].get<std::vector<std::string>>();
        return data;
    }
};
