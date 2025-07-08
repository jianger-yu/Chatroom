#include "client.h"
#include <algorithm>
#include "../socket/socket.h"
#include "../EpollReactor.hpp"
#include "menu.hpp"
#include <cstring>
#include <termios.h>  // 用于配置终端I/O
#include <unistd.h>   // 提供Unix标准API
#define CONPORT 4413

//定义数据通信伪客户端
Client dataclient;

Client:: Client()
    : fd_(socket(AF_INET, SOCK_STREAM, 0)),
      socket_(std::make_unique<Socket>(fd_)){}

Client::~ Client() {
  close(fd_);
}

Client client;

bool Client::connectToHost(const char* ip, unsigned short port) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  int ret = inet_pton(AF_INET,ip,&addr.sin_addr.s_addr);
  if(ret <= 0) return false;
  
  ret = connect(fd_,(struct sockaddr *)&addr,sizeof addr);
  if(ret == -1) return false;
  return true;
}

char charget(void){
  struct termios oldt, newt;
  char ch;
  // 获取当前终端配置
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  // 设置终端为原始模式：无回显、无需回车
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  // 读取单个字符
  ch = getchar();
  // 恢复终端配置
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}

void Client::ctlthread(){
  char input = 0;
  while(1){
    fflush(stdout); // 手动刷新标准输出缓冲区
    logmenu();
    input = charget();
    switch(input){
      case '1':break;
      case '2':break;
      case '3':break;
      default:continue;
    }
  };

}


int main(){
  sigset_t set;
  sigaddset(&set,SIGINT);
  sigaddset(&set,SIGQUIT);
  sigprocmask(SIG_BLOCK,&set,NULL);
  while(client.connectToHost("127.0.0.1", CONPORT)==false){
    printf("连接服务器2100端口失败,error:%s\n",strerror(errno));
    printf("正在重新连接...\n");
    sleep(1);
  }
  client.ctlthread();


  return 0;
}

//pthread_t tid = 0;
//pthread_create(&tid, NULL, read_clit,(void *)client.getSocket());