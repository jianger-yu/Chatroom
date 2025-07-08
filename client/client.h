#pragma once
#include <memory>
#include "../socket/socket.h"
#include "../EpollReactor.hpp"
#include "../register/readdata.hpp"
#include "../socket/socket.h"
#include <algorithm>
#include "menu.hpp"
#include <cstring>
#define CONPORT 4413


class  Client {
  public:
   Client();
  ~ Client();

  /**
   * @brief 获取当前连接的通信套接字
   * @return 返回TcpSocket指针
   */
  Socket* getSocket() const { return socket_.get(); }

  /**
   * @brief 连接到主机
   * @param ip 主机的IP地址
   * @param port 主机的端口号
   * @return 成功返回true，失败返回false
   */
  bool connectToHost(const char* ip, unsigned short port);

  //客户端运行应有功能
  void ctlthread(void);


  private:
  // 通信套接字
  int fd_;
  // 通信类对象
  std::unique_ptr<Socket> socket_;

};