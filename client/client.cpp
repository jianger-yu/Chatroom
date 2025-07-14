#include "client.h"
#include "register/register.hpp"
#include "login/login.hpp"
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

void Client::ctlthread(){
  char input = 0;
  while(1){
    fflush(stdout); // 手动刷新标准输出缓冲区
    system("clear");
    startmenu();
    input = charget();
    switch(input){
      case '1':
        login l;
        l.lgin(this);
        break;
      case '2':{
        Register r;
        r.rgst(this);
        break;
      }
      case '3':
        login f;
        f.findpwd(this);
        break;
      case '4':{
        system("clear");
        return;
      }
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
    printf("连接服务器%d端口失败,error:%s\n",CONPORT,strerror(errno));
    printf("正在重新连接...\n");
    sleep(1);
  }
  recv_running = true;
  std::thread recvThread = std::thread(recv_thread, client.getSocket());
  client.ctlthread();


  //设置标志位为 false，让 recv_thread 自行退出
  recv_running = false;
  //关闭 socket，让阻塞的 recvMsg 返回
  (client.getSocket())->mshutdown();  // 确保这个函数内部会关闭连接
  //等待线程退出
  if (recvThread.joinable()) {
      recvThread.join();
  }
  fflush(stdout);
  system("clear");
  fflush(stdout);
  return 0;
}

//pthread_t tid = 0;
//pthread_create(&tid, NULL, read_clit,(void *)client.getSocket());