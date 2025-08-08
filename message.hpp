#pragma once
#include <string>
#include "json.hpp"

using json = nlohmann::json;


struct message {
    std::string sender_uid;      // 发送者 UID
    std::string receiver_uid;    // 接收者 UID 或群 ID
    std::string content;         // 消息内容
    std::string timestamp;       // 时间戳（可使用ISO 8601格式或其他字符串）
    bool is_group = false;       // 是否为群聊消息
    bool is_file = false;        // 是否为文件消息（可扩展用）
    std::string sender_name;      // 发送者 UID
    
    // 转为 JSON 字符串
    std::string toJson() const {
        json j;
        j["sender_uid"] = sender_uid;
        j["receiver_uid"] = receiver_uid;
        j["content"] = content;
        j["timestamp"] = timestamp;
        j["is_group"] = is_group;
        j["is_file"] = is_file;
        j["sender_name"] = sender_name;
        return j.dump();
    }

    // 从 JSON 字符串构造 message 对象
    static message fromJson(const std::string& s) {
        message msg; // 默认构造，所有成员初始化为默认值
        try {
            json j = json::parse(s); // 可能抛 parse_error

            // 使用 j.value() 给出默认值，防止 key 不存在或类型不匹配时报 type_error
            msg.sender_uid   = j.value("sender_uid",   "");
            msg.receiver_uid = j.value("receiver_uid", "");
            msg.content      = j.value("content",      "");
            msg.timestamp    = j.value("timestamp",    "");
            msg.is_group     = j.value("is_group",     false);
            msg.is_file      = j.value("is_file",      false);
            msg.sender_name  = j.value("sender_name",  "");

        } catch (const json::parse_error& e) {
            std::cerr << "[JSON Parse Error] " << e.what()
                    << "\nRaw Data: [" << s << "]\n";
        } catch (const json::type_error& e) {
            std::cerr << "[JSON Type Error] " << e.what()
                    << "\nRaw Data: [" << s << "]\n";
        } catch (const std::exception& e) {
            std::cerr << "[JSON Other Error] " << e.what()
                    << "\nRaw Data: [" << s << "]\n";
        }

        return msg; // 出错时返回默认值
    }

    static std::string get_beijing_time() {
        using namespace std::chrono;

        //获取当前系统时间（UTC）
        auto now = system_clock::now();
        std::time_t now_time = system_clock::to_time_t(now);

        //添加 8 小时偏移（UTC+8）
        now_time += 8 * 60 * 60;

        std::tm* bj_time = std::gmtime(&now_time);  //转为北京时间

        std::ostringstream oss;
        oss << std::put_time(bj_time, "%Y-%m-%d %H:%M");
        return oss.str();
    }
};

class messages {
public:
    std::vector<std::string> data;

    messages() = default;

    messages(const std::vector<std::string>& msgs) : data(msgs) {}

    // 序列化为 JSON 字符串
    std::string toJson() const {
        json j;
        j["data"] = data;
        return j.dump();
    }

    // 从 JSON 字符串反序列化为 messages 对象
    static messages fromJson(const std::string& str) {
        messages m;
        try {
            json j = json::parse(str);
            m.data = j.at("data").get<std::vector<std::string>>();
        } catch (const json::parse_error& e) {
            std::cerr << "[messages::fromJson] JSON parse error: " << e.what()
                    << "\nRaw: " << str << std::endl;
        } catch (const json::out_of_range& e) {
            std::cerr << "[messages::fromJson] Missing key: " << e.what()
                    << "\nRaw: " << str << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[messages::fromJson] Error: " << e.what()
                    << "\nRaw: " << str << std::endl;
        }
        return m;
    }


};

class friendnamelist{
public:
    std::vector<std::string> data;

    friendnamelist() = default;

    friendnamelist(const std::vector<std::string>& msgs) : data(msgs) {}

    // 序列化为 JSON 字符串
    std::string toJson() const {
        json j;
        j["data"] = data;
        return j.dump();
    }

    // 从 JSON 字符串反序列化为 friendnamelist 对象
    static friendnamelist fromJson(const std::string& str) {
        friendnamelist m;
        try {
            json j = json::parse(str);
            m.data = j.at("data").get<std::vector<std::string>>();
        } catch (const json::parse_error& e) {
            std::cerr << "[friendnamelist::fromJson] JSON parse error: " << e.what()
                    << "\nRaw: " << str << std::endl;
        } catch (const json::out_of_range& e) {
            std::cerr << "[friendnamelist::fromJson] Missing key: " << e.what()
                    << "\nRaw: " << str << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[friendnamelist::fromJson] Error: " << e.what()
                    << "\nRaw: " << str << std::endl;
        }
        return m;
    }

};


struct file_block {
    std::string sender_uid;      // 发送者 UID
    std::string receiver_uid;    // 接收者 UID 或群 ID
    std::string fid;             // 文件id
    std::string filename;        // 文件名 
    std::string timestamp;       // 时间戳（可使用ISO 8601格式或其他字符串）
    int offset;
    bool is_group = false;       // 是否为群聊消息
    bool is_file = false;        // 是否为文件消息（可扩展用）
    
    // 转为 JSON 字符串
    std::string toJson() const {
        json j;
        j["sender_uid"] = sender_uid;
        j["receiver_uid"] = receiver_uid;
        j["fid"] = fid;
        j["filename"] = filename;
        j["timestamp"] = timestamp;
        j["offset"] = offset;
        j["is_group"] = is_group;
        j["is_file"] = is_file;
        return j.dump();
    }

    // 从 JSON 字符串构造 file_block 对象
    static file_block fromJson(const std::string& s) {
        file_block msg;
        try {
            json j = json::parse(s);
            msg.sender_uid = j["sender_uid"];
            msg.receiver_uid = j["receiver_uid"];
            msg.fid = j["fid"];
            msg.filename = j["filename"];
            msg.timestamp = j["timestamp"];
            msg.offset = j["offset"];
            msg.is_group = j["is_group"];
            msg.is_file = j["is_file"];
        }
        catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << "\nJSON content: " << s << std::endl;
            // 可以抛出异常，或者返回默认 msg，也可以设置标志位
            throw;
        }
        catch (const json::type_error& e) {
            std::cerr << "JSON type error: " << e.what() << "\nJSON content: " << s << std::endl;
            throw;
        }
        catch (const std::exception& e) {
            std::cerr << "JSON unknown error: " << e.what() << "\nJSON content: " << s << std::endl;
            throw;
        }
        return msg;
    }

    static std::string get_beijing_time() {
        using namespace std::chrono;

        //获取当前系统时间（UTC）
        auto now = system_clock::now();
        std::time_t now_time = system_clock::to_time_t(now);

        //添加 8 小时偏移（UTC+8）
        now_time += 8 * 60 * 60;

        std::tm* bj_time = std::gmtime(&now_time);  //转为北京时间

        std::ostringstream oss;
        oss << std::put_time(bj_time, "%Y-%m-%d %H:%M");
        return oss.str();
    }
};