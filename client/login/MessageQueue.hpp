#pragma once
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include "../../socket/socket.h"
#include <atomic>

class MessageQueue {
public:
    void push(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(msg);
        cv_.notify_one();  // 唤醒一个等待线程
    }

    std::string wait_and_pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty(); }); // 阻塞等待直到队列非空
        std::string msg = queue_.front();
        queue_.pop();
        return msg;
    }

    bool try_pop(std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) return false;
        msg = queue_.front();
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }

    bool has_data() const {
        return !queue_.empty();
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<std::string> queue_;
};

MessageQueue EchoMsgQueue;
MessageQueue ReptMsgQueue;
MessageQueue UserMsgQueue;

bool recv_running = false;

void recv_thread(Socket* sock) {
    std::string msg;
    while (recv_running) {
        sock->recvMsg(msg);  //从服务器读取一条消息
        if(msg[0] == 'e' && msg[1] == 'c' && msg[2] == 'h' && msg[3] == 'o' && msg[4] == ':')
            EchoMsgQueue.push(msg.c_str() + 5);    // 放入队列
        else if(msg[0] == 'r' && msg[1] == 'e' && msg[2] == 'p' && msg[3] == 't' && msg[4] == ':')
            ReptMsgQueue.push(msg.c_str() + 5);    // 放入队列
        else if(msg[0] == 'u' && msg[1] == 's' && msg[2] == 'e' && msg[3] == 'r' && msg[4] == ':')
            UserMsgQueue.push(msg.c_str() + 5);    // 放入队列
        else if(msg[0] == 'l' && msg[1] == 'g' && msg[2] == 'e' && msg[3] == 'x' && msg[4] == ':'){
            system("clear");
            fflush(stdout);
            printf("\033[0;31m您的账号在别处登录!\033[0m\n");
            sock->~Socket();
            exit(1);
        }
        else {
            std::cerr << "连接断开\n";
            break;
        }
    }
}
