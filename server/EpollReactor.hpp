#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <time.h>
#include <queue>
#include <pwd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "ThreadPool.hpp"
#include "./server/server.h"
#include "handle.hpp"

#define MAX_EVENTS 1024     //监听上限数
#define BUFLEN 4096
#define SERV_PORT 1145
#define MAX_PORT 65535      //端口上限
#define DATASENDIP "127.0.0.1"
#define  FILEROAD "/home/jianger/codes/ server/server/ file"
#define max_road 4096

struct my_dr{
    char name[256];
    long unsigned int ino;
};

class readctor{
private:

typedef struct event{
    int fd;         //待监听的文件描述符
    int events;     //对应的监听事件
    void*arg;       //泛型参数
    void (readctor::*call_back)(int fd, int events, void * arg); //回调函数
    int status;     //是否在红黑树上，1->在，0->不在
    //读入的信息及长度
    char buf[BUFLEN];
    int len;
    //用于监听的文件描述符
    int lisfd;
    int lisdatafd;
    //用于给客户端传输数据的文件描述符
    int datafd;
    //用于通信的地址结构
    struct sockaddr_in skaddr;
    socklen_t skaddr_len;
    //用于控制顺序的锁和条件变量及控制原子
    pthread_mutex_t pthlock;
    pthread_cond_t pthcond;

    //用于给数据传输线程通知有新任务的锁
    pthread_mutex_t datalock;
    pthread_cond_t datacond;

    pthread_mutex_t splock;
    pthread_cond_t spcond;


    //true--线程已准备好， false--线程未准备好
    bool pthready;
    bool dataready;
    bool spready;
    bool lockinit;
    long last_active;   //记录最后加入红黑树的时间值

    bool poolrs;//true---已经加入线程池
    bool poolda;//true---已经开启数据传输线程
}event;

    struct EventContext {
        event* ev;
        readctor* obj;
    };
    
    int epfd;   //红黑树的句柄
    event r_events[MAX_EVENTS + 1];
    pthread_pool pthpool;
    pthread_mutex_t event_mutex; // 事件锁，用于修改红黑树的公共区域
    std::queue<EventContext*> evq;

    //删除树上节点
    void eventdel(event * ev);
    
    //监听回调
    void acceptconn(int lfd,int tmp, void * arg);

    //获取一个端口与数据传输线程
    unsigned short getport(event * ev);
    static void data_pth(readctor::event* ev, unsigned short port, readctor* th);
    
    //获取需发送的字符串
    void getsendstr(event* ev,unsigned short dataport, std::string &str);

    //处理回调
    void senddata(int fd,int tmp, void * arg);
    
    //读回调
    void recvdata(int fd, int events, void*arg);

    //初始化事件
    void eventset(event * ev, int fd, void (readctor::* call_back)(int ,int , void *), void * arg);

    //添加文件描述符到红黑树
    void eventadd(int events, event * ev);

    //初始化监听socket
    void InitListenSocket(unsigned short port);

    void readctorinit(unsigned short port);

    //线程池执行的需要执行任务
    static void event_callback_wrapper(struct EventContext * arg) {
        struct EventContext* ctx = arg;
        (ctx->obj->*(ctx->ev->call_back))(ctx->ev->fd, ctx->ev->events, ctx->ev->arg);
    }

public:
    // 无参构造函数
    readctor();  
    // 带参构造函数
    readctor(unsigned short port);

    ~readctor();
};

void readctor::eventdel(event * ev){
    struct epoll_event epv = {0,{0}};
    if(ev -> status != 1)  //不在红黑树上
        return;
    
    epv.data.ptr = NULL;
    ev -> status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &epv);
    return ;
}

//监听回调
void readctor::acceptconn(int lfd,int tmp, void * arg){
    event* ev = (event*) arg;
    struct sockaddr_in caddr;
    socklen_t len = sizeof caddr;
    int cfd, i;
    
    ev->poolrs = false;

    printf("监听回调 准备抢 event_mutex\n");
    pthread_mutex_lock(&event_mutex); // 加锁
    printf("监听回调 抢到 event_mutex\n");

    if((cfd = accept(lfd, (struct sockaddr *)&caddr,&len)) == -1){
        if(errno != EAGAIN && errno != EINTR){
            //暂时不做出错处理
        }
        printf("accept, %s",strerror(errno));
        return;
    }
    do{
        for(i = 0; i < MAX_EVENTS ; i ++)       //从r_events中找到一个空闲位置
            if(r_events[i].status == 0)
                break;

        if(i == MAX_EVENTS){
            printf("max connect limit[%d]\n",MAX_EVENTS);
            break;
        }

        int flag = 0;
        if((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0){       //将cfd也设置为非阻塞
            printf("fcntl nonblocking failed, %s\n",strerror(errno));
            break;
        }

        eventset(&r_events[i], cfd, &readctor::recvdata, &r_events[i]);
        eventadd(EPOLLIN, &r_events[i]);
    }while(0);

    pthread_mutex_unlock(&event_mutex); // 解锁
    printf("监听回调 解开 event_mutex\n");


    printf("new connect [%s:%d][time:%ld], pos[%d]\n",
    inet_ntoa(caddr.sin_addr),ntohs(caddr.sin_port), r_events[i].last_active, i);
    return;
}


//处理回调
void readctor::senddata(int fd,int tmp, void * arg){
    event * ev = (event*)arg;
    printf("处理回调被执行,ev->buf:%s\n",ev->buf);

    handler hand(ev->buf, fd);
    hand.handle();
    
    printf("senddata 准备抢 event_mutex\n");
    pthread_mutex_lock(&event_mutex); // 修改红黑树公共区域，加事件锁
    printf("senddata 抢到 event_mutex\n");
   
    eventdel(ev);
    
    memset(ev->buf, 0, sizeof ev->buf);
    eventset(ev,fd,&readctor::recvdata,ev);
    eventadd(EPOLLIN, ev);   

    pthread_mutex_unlock(&event_mutex); // 解锁
    printf("senddata 解除 event_mutex\n");

}


//读回调
void readctor::recvdata(int fd, int events, void*arg){
    event *ev = (event *) arg;
    int len;
    std::string str;
    printf("recvdata 准备 recvMsg\n");
    int ret = recvMsg(str, fd);

    printf("recvdata 准备抢 event_mutex\n");
    pthread_mutex_lock(&event_mutex); // 加锁
    printf("recvdata 抢到 event_mutex\n");

    if(ret == -1){//失败处理
        close(ev->fd);
        printf("recvMsg[fd = %d] error[%d]:%s\n",fd,errno,strerror(errno));
        pthread_mutex_unlock(&event_mutex); // 解锁
        return;
    }
    memset(ev->buf, 0, sizeof ev->buf);
    strcpy(ev->buf,str.c_str());
    len = str.size();

    eventdel(ev);//将该节点从红黑树摘除

    if(len > 0){
        ev->len = len;
        ev->buf[len] ='\0';
        printf("C[%d]:%s",fd,ev->buf);

        eventset(ev,fd,&readctor::senddata,ev);    //设置该fd对应的回调函数为senddata
        eventadd(EPOLLOUT, ev);         //将fd加入红黑树中，监听其写事件

    } else if(len == 0){//对端已关闭
        close(ev->fd);
        printf("[fd = %d] pos[%ld], closed\n", fd, ev-r_events);
    }else{
        close(ev->fd);
        printf("recv[fd = %d] str.size() == [%d] error[%d]:%s\n",fd,len,errno,strerror(errno));
    }

    pthread_mutex_unlock(&event_mutex); // 解锁
    printf("recvdata 解除 event_mutex\n");

}

//初始化事件
void readctor::eventset(event * ev, int fd, void (readctor::* call_back)(int ,int , void *), void * arg){
    ev -> fd = fd;
    ev -> call_back = call_back;
    ev -> arg = arg;

    ev -> events = 0;
    ev -> status = 0; 

    if(ev->lockinit == false){
        pthread_mutex_init(&ev->pthlock, NULL);
        pthread_cond_init(&ev->pthcond, NULL);

        pthread_mutex_init(&ev->datalock, NULL);
        pthread_cond_init(&ev->datacond, NULL);
        
        pthread_mutex_init(&ev->splock, NULL);
        pthread_cond_init(&ev->spcond, NULL);

        ev->lockinit = true;
    }

    ev->poolrs = false;
   
    ev -> last_active = time(NULL);     //调用eventset函数的时间
    return;
}

//添加文件描述符到红黑树
void readctor::eventadd(int events, event * ev){
    //事件处理采用ET模式
    int combined_events = events | EPOLLET;
    //events |= EPOLLET;
    struct epoll_event epv = { 0 , { 0 }};
    int op = EPOLL_CTL_MOD;
    epv.data.ptr = ev;
    epv.events = ev -> events = combined_events;

    if(ev -> status == 0){      //若ev不在树内
        op = EPOLL_CTL_ADD;
        ev -> status = 1;
    }
    else{
        if(epoll_ctl(epfd,op,ev -> fd, &epv) < 0)
            printf("epoll_ctl  mod is error :[fd = %d], events[%d]\n", ev->fd, combined_events);
        else
            printf("epoll_ctl mod sccess on [fd = %d], [op = %d] events[%0X]\n",ev->fd, op, combined_events);
        return;
    }
    if(epoll_ctl(epfd, op, ev -> fd, &epv) < 0)
        printf("epoll_ctl is error :[fd = %d], events[%d]\n", ev->fd, combined_events);
    else
        printf("epoll_ctl sccess on [fd = %d], [op = %d] events[%0X]\n",ev->fd, op, combined_events);
}

//初始化监听socket
void readctor::InitListenSocket(unsigned short port){
    struct sockaddr_in addr;
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {//端口复用
        perror("setsockopt failed");
        close(lfd);
        return ;
    }
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    memset(&addr, 0, sizeof addr);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    bind(lfd,(sockaddr *)&addr, sizeof addr);

    listen(lfd, 100);

    eventset(&r_events[MAX_EVENTS], lfd, &readctor::acceptconn, &r_events[MAX_EVENTS]);
    eventadd(EPOLLIN, &r_events[MAX_EVENTS]);
}



void readctor::readctorinit(unsigned short port){
    pthread_mutex_init(&event_mutex,NULL);
    signal(SIGPIPE, SIG_IGN); 
    epfd = epoll_create(MAX_EVENTS + 1);            //定义最大节点数为MAX_EVENTS + 1的红黑树
    if(epfd <= 0)
        printf("epfd create is error, epfd : %d\n", epfd);
    InitListenSocket(port);                         //初始化套接字

    struct epoll_event events[MAX_EVENTS + 1];      //保存已经满足就绪事件的文件描述符
    printf("server running port:[%d]\n", port);
    int chekckpos = 0, i;

    while(1){
        //↓↓↓超时验证
        long now = time(NULL);
        for(i = 0; i < 100; i++, chekckpos++){       //一次循环检验100个，chekckpos控制检验对象
            if(chekckpos == MAX_EVENTS)
                chekckpos = 0;
            if(r_events[chekckpos].status != 1)      //不在红黑树上，继续循环
                continue;
            
            long duration = now -r_events[chekckpos].last_active;   //计算客户端不活跃的时间
            if(duration >= 6000){
                printf("[fd = %d] timeout\n", r_events[chekckpos].fd);
                pthread_mutex_lock(&event_mutex); // 加锁
                eventdel(&r_events[chekckpos]);
                close(r_events[chekckpos].fd);
                pthread_mutex_unlock(&event_mutex); // 加锁
            }
        }
        //↑↑↑超时验证
        //监听红黑树epfd，将满足的事件的文件描述符加至events数组中，1秒没有文件满足，则返回0
        int nfd = epoll_wait(epfd, events, MAX_EVENTS + 1, 1000); 
        if(nfd < 0){
            printf("epoll_wait error :%s\n",strerror(errno));
            continue;
        }

        for(i = 0; i < nfd; i++){
            event *ev = (event *) events[i].data.ptr;
            //读事件，调用读回调
            if(!ev->poolrs){
                ev->poolrs = true;
                if((events[i].events & EPOLLIN) && (ev -> events & EPOLLIN)){
                    struct EventContext* ctx = (struct EventContext*)malloc(sizeof (struct EventContext));
                    ctx->ev = ev;
                    ctx->obj = this;
                    evq.push(ctx);
                    pthpool.PushTask(event_callback_wrapper, ctx);
                }
                //写事件，调用写回调
                if((events[i].events & EPOLLOUT) && (ev -> events & EPOLLOUT)){
                    struct EventContext* ctx = (struct EventContext*)malloc(sizeof (struct EventContext));
                    ctx->ev = ev;
                    ctx->obj = this;
                    evq.push(ctx);
                    pthpool.PushTask(event_callback_wrapper, ctx);
                }
            }
        }
    }
}

// 无参构造函数
readctor::readctor(){
    unsigned short port = SERV_PORT;
    readctorinit(port);
}          
// 带参构造函数
readctor::readctor(unsigned short port){
    readctorinit(port);
}   

readctor::~readctor() {
    pthread_mutex_destroy(&event_mutex);
    // 1. 停止线程池并等待所有任务完成
    pthpool.~pthread_pool(); // 调用线程池析构函数（若线程池未自动管理，可显式停止）
    
    // 2. 关闭 epoll 句柄
    if (epfd > 0) {
        close(epfd);
        epfd = -1;
    }

    // 3. 关闭所有客户端套接字（遍历 r_events）
    for (int i = 0; i < MAX_EVENTS + 1; i++) {
        event& ev = r_events[i];
        if (ev.status == 1 && ev.fd > 0) { // 若在红黑树上且fd有效
            eventdel(&ev); // 从epoll中删除
            close(ev.fd); // 关闭套接字
            ev.fd = -1;
            ev.status = 0;
        }
    }

    // 4. 清理监听套接字（位于 r_events[MAX_EVENTS]）
    event& listen_ev = r_events[MAX_EVENTS];
    if (listen_ev.fd > 0) {
        eventdel(&listen_ev);
        close(listen_ev.fd);
        listen_ev.fd = -1;
    }

    //释放队列
    while(!evq.empty()){
        free(evq.front());
        evq.pop();
    }
}
