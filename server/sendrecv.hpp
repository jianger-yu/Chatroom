#pragma once
#include<iostream>
#include<cstring>
#include<string>
#include <sys/socket.h>
#include <netinet/in.h>


bool send_all(int sockfd,const void * buf,size_t len){
  const char*p = static_cast<const char*>(buf);
  while(len > 0){
    int n = send(sockfd,p,len,0);
    if(n <= 0) return false;
    p += n;
    len -= n;
  }
  return true;
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

int sendMsg(std::string msg,int sockfd_) {
  uint32_t len = htonl(msg.size());
  if(!send_all(sockfd_,&len,sizeof len)) return -1;
  if(!send_all(sockfd_,msg.data(),msg.size())) return -1;
  return 0;
}

int recvMsg(std::string& msg,int sockfd_) {
  uint32_t len, slen;
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
  return 0;
}