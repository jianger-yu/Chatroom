#pragma once
#include <string>
#include <set>
#include "json.hpp"

using json = nlohmann::json;

struct group {
    std::string gid;
    std::string name;
    std::string owner;
    std::set<std::string> managelist;
    std::set<std::string> memberlist;

    // 将当前对象转为 JSON 字符串
    std::string toJson() const {
        json j;
        j["gid"] = gid;
        j["name"] = name;
        j["owner"] = owner;
        j["managelist"] = managelist;
        j["memberlist"] = memberlist;
        return j.dump();
    }

    // 从 JSON 字符串构造对象
    static group fromJson(const std::string& s) {
        json j = json::parse(s);
        group data;
        data.gid = j["gid"];
        data.name = j["name"];
        data.owner = j["owner"];
        data.managelist = j["managelist"].get<std::set<std::string>>();
        data.memberlist = j["memberlist"].get<std::set<std::string>>();
        return data;
    }
};
