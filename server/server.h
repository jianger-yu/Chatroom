#pragma once
#include <memory>
#include "../socket/socket.h"
#include <unordered_map>
#include "../Logger.h"

class server {
  public:
  server();
  ~server();

  /**
   * @brief 获取当前连接的通信套接字
   * @return 返回TcpSocket指针
   */
  Socket* getSocket() const { return socket_.get(); }


  /**
   * @brief 设置服务器监听端口
   * @param port 监听的端口号
   * @return 成功返回true，失败返回false
   */
  bool setListen(unsigned short port);


  /**
   * @brief 接受客户端连接
   * @return 成功返回true，失败返回false
   */
  bool acceptConn();


  private:
  // 监听套接字
  int listenfd_;
  // 地址结构
  struct sockaddr_in addr;
  socklen_t addr_len;
  // 通信类对象
  std::unique_ptr<Socket> socket_;
};

std::unordered_map<std::string, int> uid_to_socket;
std::unordered_map<int, std::string> socket_to_uid;
