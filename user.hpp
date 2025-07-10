#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

struct user {
    std::string name;
    std::string email;
    std::string pwd;
    std::string stat;
    std::vector<std::string> frd;
    std::vector<std::string> group;

    //将当前对象转为JSON字符串
    std::string toJson() const {
        json j;
        j["name"] = name;
        j["email"] = email;
        j["pwd"] = pwd;
        j["stat"] = stat;
        j["frd"] = frd;
        j["group"] = group;
        return j.dump();
    }

    //从JSON字符串构造对象
    static user fromJson(const std::string& s) {
        json j = json::parse(s);
        user data;
        data.name = j["name"];
        data.email = j["email"];
        data.pwd = j["pwd"];
        data.stat = j["stat"];
        data.frd = j["frd"].get<std::vector<std::string>>();
        data.group = j["group"].get<std::vector<std::string>>();
        return data;
    }
};
