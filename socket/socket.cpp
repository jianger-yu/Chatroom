#include "socket.h"

Socket::Socket(int sockfd) : sockfd_(sockfd) {}

Socket::~Socket() {
  close(sockfd_);
}

bool Socket::send_all(int sockfd,const void * buf,size_t len){
  const char*p = static_cast<const char*>(buf);
  while(len > 0){
    int n = send(sockfd,p,len,0);
    if(n <= 0) return false;
    p += n;
    len -= n;
  }
  return true;
}

int Socket::sendMsg(std::string msg) {
  uint32_t len = htonl(msg.size());
  if(!send_all(sockfd_,&len,sizeof len)) return -1;
  if(!send_all(sockfd_,msg.data(),msg.size())) return -1;
  return 0;
}

bool Socket::recv_all(int sockfd,void * buf,size_t len){
  char* p = static_cast<char*>(buf);
  while(len > 0){
    int n = recv(sockfd,p,len,0);
    if(n < 0) return false;
    if(n == 0){
      printf("已断开连接\n");
      //return false;
      exit(1);
    }
    p += n;
    len -= n;
  }
  return true;
}

int Socket::recvMsg(std::string& msg) {
  uint32_t len, slen;
  if(!recv_all(sockfd_,&len,sizeof len)) return -1;
  slen = ntohl(len);
  msg.clear();
  msg.resize(slen);
  if(!recv_all(sockfd_,msg.data(),slen)) return -1;
  return 0;
}

void Socket::setNonBlocking() {
    int flags = fcntl(sockfd_, F_GETFL, 0);
    if (flags == -1) return;
    fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);
}


bool Socket::send_allfile(int sockfd, const void* buf, size_t len) {
    const char* p = static_cast<const char*>(buf);
    int retry = 0;
    const int MAX_RETRY = 10000;

    while (len > 0) {
        int n = send(sockfd, p, len, 0);
        if (n > 0) {
            p += n;
            len -= n;
            retry = 0;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (++retry > MAX_RETRY) return false;
            usleep(1000); // 等待缓冲区腾出空间
            printf("重试次数:%d", retry);
        } else {
            return false; // 连接异常
        }
    }
    return true;
}


int Socket::sendFILE(const std::string& msg) {
    uint32_t len = htonl(msg.size());
    if (!send_allfile(sockfd_, &len, sizeof len)) return -1;
    if (!send_allfile(sockfd_, msg.data(), msg.size())) return -1;
    return 0;
}


int Socket::recvfull(std::string& str) {
    char buf[4096];
    while (true) {
        ssize_t n = recv(sockfd_, buf, sizeof(buf), 0);
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