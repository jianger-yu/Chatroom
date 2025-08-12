#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mutex>

extern std::unordered_map<int, std::unique_ptr<std::mutex>> fd_write_mutexes;
extern std::unordered_map<int, std::unique_ptr<std::mutex>> fd_read_mutexes;
extern std::unordered_map<std::string, time_t> last_active;

bool send_all(int sockfd,const void * buf,size_t len){
  const char*p = static_cast<const char*>(buf);
  while(len > 0){
    int n = send(sockfd,p,len,0);
    if (n <= 0) {
        perror("send failed:");
        return false;
    }
    //if(n <= 0) return false;
    p += n;
    len -= n;
  }
  return true;
}

int sendMsg(std::string msg,int sockfd_) {
  uint32_t len = htonl(msg.size());
  auto it = fd_write_mutexes.find(sockfd_);
  if (it != fd_write_mutexes.end()) {
      std::lock_guard<std::mutex> lock(*(it->second));
      if(!send_all(sockfd_,&len,sizeof len)) return -1;
      if(!send_all(sockfd_,msg.data(),msg.size())) return -1;
  }
  return 0;
}

int recv_all(int sockfd,void * buf,size_t len){
  char* p = static_cast<char*>(buf);
  int n;
  while(len > 0){
    do {
        n = recv(sockfd,p,len,0);
        if (n > 0) { p += n; len -= n; }
        else if (n == 0) {
            //对端关闭
            len = 0;
            return 10;
        }
        else if (errno != EAGAIN && errno != EWOULDBLOCK) return false;
        if(len == 0) break;
    }
    while(n > 0);
    //if(!(errno == EAGAIN || errno == EWOULDBLOCK)) return false; 
  }
  return true;
}

int recvMsg(std::string& msg,int sockfd_) {
  uint32_t len, slen;
  auto it = fd_read_mutexes.find(sockfd_);
  if (it != fd_read_mutexes.end()) {
    std::lock_guard<std::mutex> lock(*(it->second));
    int ret = recv_all(sockfd_,&len,sizeof len);
    if(!ret) return -1;
    else if(ret == 10)
      return 10;
    slen = ntohl(len);
    msg.clear();
    msg.resize(slen);
    ret = recv_all(sockfd_,msg.data(),slen);
    if(!ret) return -1;
    else if(ret == 10)
      return 10;
  }
  return 0;
}

int recvfull(std::string& str, int sockfd) {
    char buf[4096];
    while (true) {
        ssize_t n = recv(sockfd, buf, sizeof(buf), 0);
        if (n > 0) {
            str.append(buf, n); // 追加读到的数据
        } else if (n == 0) {
            // 对端关闭连接
            return 10; // 约定的关闭标志
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 读完了，缓冲区空
                break;
            } else if (errno == EINTR) {
                // 信号中断，继续读
                continue;
            } else {
                // 其他错误
                return -1;
            }
        }
    }
    return 0;
}


bool send_allfile(int sockfd, const void* buf, size_t len) {
    const char* p = static_cast<const char*>(buf);
    int retry = 0;
    const int MAX_RETRY = 100000;

    while (len > 0) {
        int n = send(sockfd, p, len, 0);
        if (n > 0) {
            p += n;
            len -= n;
            retry = 0;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (++retry > MAX_RETRY) return false;
            usleep(1000); // 等待缓冲区腾出空间
            //printf("重试次数:%d", retry);
        } else {
            return false; // 连接异常
        }
    }
    return true;
}


int sendFILE(const std::string& msg,int sockfd_) {
    uint32_t len = htonl(msg.size());
    if (!send_allfile(sockfd_, &len, sizeof len)) return -1;
    if (!send_allfile(sockfd_, msg.data(), msg.size())) return -1;
    return 0;
}

