#pragma once
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
#include <mutex>
#include <thread>

#include "ThreadPool.hpp"
#include "./server/server.h"
#include "handle.hpp"

#define MAX_EVENTS 1024     //监听上限数
#define MAX_PORT 65535      //端口上限

std::string server_ip_ = "10.30.0.109";
uint16_t server_port_ = 4413;

extern std::unordered_map<int, std::unique_ptr<std::mutex>> fd_write_mutexes;
extern std::unordered_map<int, std::unique_ptr<std::mutex>> fd_read_mutexes;
extern std::unordered_map<std::string, time_t> last_active;

class WorkerReactor{
private:

typedef struct event{
    int fd;         //待监听的文件描述符
    int events;     //对应的监听事件
    void*arg;       //泛型参数
    void (WorkerReactor::*call_back)(int fd, int events, void * arg); //回调函数
    int status;     //是否在红黑树上，1->在，0->不在
    //读入的信息及长度
    std::string buf;
    std::mutex buf_mutex; //缓冲区锁

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
        WorkerReactor* obj;
    };
    
    int epfd;   //红黑树的句柄
    event r_events[MAX_EVENTS + 1];
    pthread_pool &pthpool;
    pthread_mutex_t event_mutex; // 事件锁，用于修改红黑树的公共区域
    std::queue<EventContext*> evq;
    //标记是否为用于数据传输的反应堆
    bool datareactor = false;

    //删除树上节点
    void eventdel(event * ev);
    
    //监听回调
    void acceptconn(int lfd,int tmp, void * arg);

    //获取一个端口与数据传输线程
    unsigned short getport(event * ev);
    static void data_pth(WorkerReactor::event* ev, unsigned short port, WorkerReactor* th);
    
    //获取需发送的字符串
    void getsendstr(event* ev,unsigned short dataport, std::string &str);

    //处理回调
    void senddata(int fd,int tmp, void * arg);
    
    //读回调
    void recvdata(int fd, int events, void*arg);


    //初始化事件
    void eventset(event * ev, int fd, void (WorkerReactor::* call_back)(int ,int , void *), void * arg);

    //添加文件描述符到红黑树
    void eventadd(int events, event * ev);

    //初始化监听socket
    void InitListenSocket(unsigned short port);


    //线程池执行的需要执行任务
    static void event_callback_wrapper(struct EventContext * arg) {
        struct EventContext* ctx = arg;
        (ctx->obj->*(ctx->ev->call_back))(ctx->ev->fd, ctx->ev->events, ctx->ev->arg);
    }

public:
    // 无参构造函数
    WorkerReactor();  
    // 带参构造函数
    WorkerReactor(unsigned short port);
    WorkerReactor(unsigned short port, pthread_pool& sharedPool, bool datarct = false);

    ~WorkerReactor();
    void addClient(int cfd, sockaddr_in client_addr); 
    void readctorinit(unsigned short port);
};


class MainReactor{
private:
    static const int WORKER_COUNT = 4;
    std::vector<std::unique_ptr<WorkerReactor>> workers;
    int next_worker = 0;  // 轮询分配新连接

typedef struct event{
    int fd;         //待监听的文件描述符
    int events;     //对应的监听事件
    void*arg;       //泛型参数
    void (MainReactor::*call_back)(int fd, int events, void * arg); //回调函数
    int status;     //是否在红黑树上，1->在，0->不在
    //读入的信息及长度
    std::string buf;
    std::mutex buf_mutex; //缓冲区锁

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
        MainReactor* obj;
    };
    
    int epfd;   //红黑树的句柄
    event r_events[MAX_EVENTS + 1];
    pthread_pool pthpool;
    pthread_mutex_t event_mutex; // 事件锁，用于修改红黑树的公共区域
    std::queue<EventContext*> evq;
    //标记是否为用于数据传输的反应堆
    bool datareactor = false;

    //删除树上节点
    void eventdel(event * ev);
    
    //监听回调
    void fileacceptconn(int lfd,int tmp, void * arg);
    void acceptconn(int lfd,int tmp, void * arg);

    //获取一个端口与数据传输线程
    unsigned short getport(event * ev);
    static void data_pth(MainReactor::event* ev, unsigned short port, MainReactor* th);
    
    //获取需发送的字符串
    void getsendstr(event* ev,unsigned short dataport, std::string &str);

    //处理回调
    void senddata(int fd,int tmp, void * arg);
    
    //读回调
    void recvdata(int fd, int events, void*arg);

    //初始化事件
    void eventset(event * ev, int fd, void (MainReactor::* call_back)(int ,int , void *), void * arg);

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
    MainReactor();  
    // 带参构造函数
    MainReactor(unsigned short port);
    MainReactor(unsigned short port, unsigned int pthread_cnt, bool datarct = false);

    ~MainReactor();
};


void MainReactor::eventdel(event * ev){
    struct epoll_event epv = {0,{0}};
    if(ev -> status != 1)  //不在红黑树上
        return;
    
    epv.data.ptr = NULL;
    ev -> status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &epv);
    return ;
}

void MainReactor::fileacceptconn(int lfd,int tmp, void * arg){
    event* ev = (event*) arg;
    struct sockaddr_in caddr;
    socklen_t len = sizeof caddr;
    int cfd, i;
    
    ev->poolrs = false;

    pthread_mutex_lock(&event_mutex); // 加锁

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

        eventset(&r_events[i], cfd, &MainReactor::recvdata, &r_events[i]);
        eventadd(EPOLLIN, &r_events[i]);
    }while(0);

    pthread_mutex_unlock(&event_mutex); // 解锁


    printf("new connect [%s:%d][time:%ld], pos[%d]\n",
    inet_ntoa(caddr.sin_addr),ntohs(caddr.sin_port), r_events[i].last_active, i);
    return;
}

//监听回调
void MainReactor::acceptconn(int lfd,int tmp, void * arg){
    event* ev = (event*) arg;
    printf("监听回调被触发\n");
    ev->poolrs = false;
    struct sockaddr_in caddr;
    socklen_t len = sizeof caddr;
    
    int cfd = accept(lfd, (struct sockaddr*)&caddr, &len);
    if (cfd == -1) {
        if (errno != EAGAIN && errno != EINTR) {
            perror("accept");
        }
        return;
    }
    fcntl(cfd, F_SETFL, O_NONBLOCK);

    // 轮询分配给一个 WorkerReactor
    int idx = next_worker;
    next_worker = (next_worker + 1) % WORKER_COUNT;
    workers[idx]->addClient(cfd, caddr);

    printf("MainReactor 分配 fd:%d 给 Worker %d [%s:%d]\n",
    cfd, idx, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
}


//处理回调
void MainReactor::senddata(int fd,int tmp, void * arg){
    event * ev = (event*)arg;
    int ret = 0;
    while(1){
        std::string str;
        {
            std::lock_guard<std::mutex> lock(ev->buf_mutex);
            if (ev->buf.size() < 4) {
                break; // 不够完整包
            }
            uint32_t len, slen;
            std::memcpy(&len, ev->buf.data(), sizeof(len));
            slen = ntohl(len);
            if(ev->buf.size() < 4 + slen) break;
            str = ev->buf.substr(4, slen);
            ev->buf.erase(0, 4+slen);
            printf("处理回调取出slen:%d, str,size():%ld, str:%s\n", slen,str.size(),str.c_str());
        }
        
        if(datareactor && str[0] == 'r' && str[1] == 'v' && str[2] == 'f' && str[3] == 'l') rvfl(str);
        else if(datareactor && str[0] == 'r' && str[1] == 'v' && str[2] == 'g' && str[3] == 'f') rvgf(str);
        else {
            handler hand(str, fd);
            if(datareactor && str[0] == 's' && str[1] == 'd' && str[2] == 'f' && str[3] == 'l') hand.sdfl();
            else if(datareactor && str[0] == 's' && str[1] == 'd' && str[2] == 'g' && str[3] == 'f') hand.sdgf();
            else ret = hand.handle();
        }
    }
    printf("handle处理完毕\n");
    pthread_mutex_lock(&event_mutex); // 修改红黑树公共区域，加事件锁

    eventdel(ev);
    if(ret == 1) {
        printf("fd:%d 被关闭!\n", fd);
        close(fd);
        pthread_mutex_unlock(&event_mutex); // 解锁
        return;
    }
    //memset(ev->buf, 0, sizeof ev->buf);
    eventset(ev,fd,&MainReactor::recvdata,ev);
    eventadd(EPOLLIN, ev);   

    pthread_mutex_unlock(&event_mutex); // 解锁
}


//读回调
void MainReactor::recvdata(int fd, int events, void*arg){
    event *ev = (event *) arg;
    int len;
    std::string str;
    int ret = recvfull(str, fd);
    if(ret == 10){
        if(!datareactor){
            std::string uid = socket_to_uid[fd];
            printf("\033[0;31mrecvfull 判定 uid:%s断连, 正在关闭对应fd:%d\033[0m\n", uid.c_str(), fd);
            str = "rvlg:" + uid;
            uint32_t slen = htonl(str.size());
            str.insert(0, reinterpret_cast<const char*>(&slen), sizeof(slen));
        }
    }
    pthread_mutex_lock(&event_mutex); // 加锁

    if(ret == -1){//失败处理
        close(ev->fd);
        printf("recvMsg[fd = %d] error[%d]:%s\n",fd,errno,strerror(errno));
        pthread_mutex_unlock(&event_mutex); // 解锁
        return;
    }
    {
        std::lock_guard<std::mutex> lock(ev->buf_mutex);
        ev->buf.append(str);
    }
    // memset(ev->buf, 0, sizeof ev->buf);
    // memcpy(ev->buf, str.data(), str.size());
    len = str.size();

    eventdel(ev);//将该节点从红黑树摘除

    if(len > 0){
        ev->len = len;
        //ev->buf[len] ='\0';
        printf("C[fd:%d], ev->buf,size():%ld\n",fd,ev->buf.size());

        eventset(ev,fd,&MainReactor::senddata,ev);    //设置该fd对应的回调函数为senddata
        eventadd(EPOLLOUT, ev);         //将fd加入红黑树中，监听其写事件

    } else if(len == 0){//对端已关闭
        close(ev->fd);
        printf("[fd = %d] pos[%ld], closed\n", fd, ev-r_events);
    }else{
        close(ev->fd);
        printf("recv[fd = %d] str.size() == [%d] error[%d]:%s\n",fd,len,errno,strerror(errno));
    }

    pthread_mutex_unlock(&event_mutex); // 解锁

}


//初始化事件
void MainReactor::eventset(event * ev, int fd, void (MainReactor::* call_back)(int ,int , void *), void * arg){
    ev -> fd = fd;
    fd_write_mutexes[fd] = std::make_unique<std::mutex>();
    fd_read_mutexes[fd] = std::make_unique<std::mutex>();

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
void MainReactor::eventadd(int events, event * ev){
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
void MainReactor::InitListenSocket(unsigned short port){
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
    //addr.sin_addr.s_addr = INADDR_ANY;    //绑定网卡上任意ip
    inet_pton(AF_INET, server_ip_.c_str(), &addr.sin_addr); //连接指定端口

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    bind(lfd,(sockaddr *)&addr, sizeof addr);

    listen(lfd, 100);

    if(!datareactor)
        eventset(&r_events[MAX_EVENTS], lfd, &MainReactor::acceptconn, &r_events[MAX_EVENTS]);
    else 
        eventset(&r_events[MAX_EVENTS], lfd, &MainReactor::fileacceptconn, &r_events[MAX_EVENTS]);
    eventadd(EPOLLIN, &r_events[MAX_EVENTS]);
}



void MainReactor::readctorinit(unsigned short port){
    pthread_mutex_init(&event_mutex,NULL);
    signal(SIGPIPE, SIG_IGN); 
    epfd = epoll_create(MAX_EVENTS + 1);            //定义最大节点数为MAX_EVENTS + 1的红黑树
    if(epfd <= 0)
        printf("epfd create is error, epfd : %d\n", epfd);
    InitListenSocket(port);                         //初始化套接字

    struct epoll_event events[MAX_EVENTS + 1];      //保存已经满足就绪事件的文件描述符
    printf("server running port:[%d]\n", port);
    int chekckpos = 0, i;

    if(!datareactor){
        for(int i = 0; i < WORKER_COUNT; i++){
            //创建4个从reactor线程，存到数组中
            auto worker = std::make_unique<WorkerReactor>(0, pthpool, false);
            std::thread t([worker_ptr = worker.get()](){
                worker_ptr->readctorinit(0);
            });
            t.detach();  // 分离线程，后台运行

            workers.push_back(std::move(worker));
        }
    }
    while(1){
        //↓↓↓超时验证
        long now = time(NULL);
        for(i = 0; i < 100; i++, chekckpos++){       //一次循环检验100个，chekckpos控制检验对象
            if(chekckpos == MAX_EVENTS)
                chekckpos = 0;
            if(r_events[chekckpos].status != 1)      //不在红黑树上，继续循环
                continue;
            
            long duration = now - r_events[chekckpos].last_active;   //计算客户端不活跃的时间
            if(duration >= 3600){//超时，断连
                printf("[fd = %d] timeout\n", r_events[chekckpos].fd);
                pthread_mutex_lock(&event_mutex); // 加锁
                if(!datareactor){
                    std::string uid = socket_to_uid[r_events[chekckpos].fd];
                    handler hd({}, r_events[chekckpos].fd);
                    hd.uulg(uid);
                }
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
MainReactor::MainReactor(){
    unsigned short port = server_port_;
    readctorinit(port);
}          
// 带参构造函数
MainReactor::MainReactor(unsigned short port){
    readctorinit(port);
}   
MainReactor::MainReactor(unsigned short port, unsigned int pthread_cnt, bool datarct):pthpool(pthread_cnt),datareactor(datarct){
    readctorinit(port);
}   

MainReactor::~MainReactor() {
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

// ===================================================================================================================

void WorkerReactor::addClient(int cfd, sockaddr_in client_addr) {
    pthread_mutex_lock(&event_mutex); // 加锁
    int i;
    for (i = 0; i < MAX_EVENTS; i++) {
        if (r_events[i].status == 0) break;
    }
    if (i == MAX_EVENTS) {
        printf("Worker max connect limit\n");
        close(cfd);
        return;
    }
    fcntl(cfd, F_SETFL, O_NONBLOCK);
    eventset(&r_events[i], cfd, &WorkerReactor::recvdata, &r_events[i]);
    eventadd(EPOLLIN, &r_events[i]);
    pthread_mutex_unlock(&event_mutex); // 解锁
}

void WorkerReactor::eventdel(event * ev){
    struct epoll_event epv = {0,{0}};
    if(ev -> status != 1)  //不在红黑树上
        return;
    
    epv.data.ptr = NULL;
    ev -> status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &epv);
    return ;
}

//监听回调
void WorkerReactor::acceptconn(int lfd,int tmp, void * arg){
    event* ev = (event*) arg;
    struct sockaddr_in caddr;
    socklen_t len = sizeof caddr;
    int cfd, i;
    
    ev->poolrs = false;

    pthread_mutex_lock(&event_mutex); // 加锁

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

        eventset(&r_events[i], cfd, &WorkerReactor::recvdata, &r_events[i]);
        eventadd(EPOLLIN, &r_events[i]);
    }while(0);

    pthread_mutex_unlock(&event_mutex); // 解锁


    printf("new connect [%s:%d][time:%ld], pos[%d]\n",
    inet_ntoa(caddr.sin_addr),ntohs(caddr.sin_port), r_events[i].last_active, i);
    return;
}


//处理回调
void WorkerReactor::senddata(int fd,int tmp, void * arg){
    event * ev = (event*)arg;
    int ret = 0;
    while(1){
        std::string str;
        {
            std::lock_guard<std::mutex> lock(ev->buf_mutex);
            if (ev->buf.size() < 4) {
                break; // 不够完整包
            }
            uint32_t len, slen;
            std::memcpy(&len, ev->buf.data(), sizeof(len));
            slen = ntohl(len);
            if(ev->buf.size() < 4 + slen) break;
            str = ev->buf.substr(4, slen);
            ev->buf.erase(0, 4+slen);
            printf("处理回调取出slen:%d, str,size():%ld, str:%s\n", slen,str.size(),str.c_str());
        }
        if(str[0] == 'P' && str[1] == 'I' && str[2] == 'N' && str[3] == 'G'){
            time_t now = time(nullptr);
            last_active[socket_to_uid[fd]] = now;  // 插入或更新
            std::cout << "更新用户 " << socket_to_uid[fd] << " 的活跃时间为 " << ctime(&now);
        }
        else if(datareactor && str[0] == 'r' && str[1] == 'v' && str[2] == 'f' && str[3] == 'l') rvfl(str);
        else if(datareactor && str[0] == 'r' && str[1] == 'v' && str[2] == 'g' && str[3] == 'f') rvgf(str);
        else {
            handler hand(str, fd);
            if(datareactor && str[0] == 's' && str[1] == 'd' && str[2] == 'f' && str[3] == 'l') hand.sdfl();
            else if(datareactor && str[0] == 's' && str[1] == 'd' && str[2] == 'g' && str[3] == 'f') hand.sdgf();
            else ret = hand.handle();
        }
    }
    printf("handle处理完毕\n");
    pthread_mutex_lock(&event_mutex); // 修改红黑树公共区域，加事件锁

    eventdel(ev);
    if(ret == 1) {
        printf("fd:%d 被关闭!\n", fd);
        close(fd);
        pthread_mutex_unlock(&event_mutex); // 解锁
        return;
    }
    //memset(ev->buf, 0, sizeof ev->buf);
    eventset(ev,fd,&WorkerReactor::recvdata,ev);
    eventadd(EPOLLIN, ev);   

    pthread_mutex_unlock(&event_mutex); // 解锁
}


//读回调
void WorkerReactor::recvdata(int fd, int events, void*arg){
    event *ev = (event *) arg;
    int len;
    std::string str;
    int ret = recvfull(str, fd);
    if(ret == 10){
        if(!datareactor){
            std::string uid = socket_to_uid[fd];
            printf("\033[0;31mrecvfull 判定 uid:%s断连, 正在关闭对应fd:%d\033[0m\n", uid.c_str(), fd);
            str = "rvlg:" + uid;
            uint32_t slen = htonl(str.size());
            str.insert(0, reinterpret_cast<const char*>(&slen), sizeof(slen));
        }
    }
    pthread_mutex_lock(&event_mutex); // 加锁

    if(ret == -1){//失败处理
        close(ev->fd);
        printf("recvMsg[fd = %d] error[%d]:%s\n",fd,errno,strerror(errno));
        pthread_mutex_unlock(&event_mutex); // 解锁
        return;
    }
    {
        std::lock_guard<std::mutex> lock(ev->buf_mutex);
        ev->buf.append(str);
    }
    // memset(ev->buf, 0, sizeof ev->buf);
    // memcpy(ev->buf, str.data(), str.size());
    len = str.size();

    eventdel(ev);//将该节点从红黑树摘除

    if(len > 0){
        ev->len = len;
        //ev->buf[len] ='\0';
        printf("C[fd:%d], ev->buf,size():%ld\n",fd,ev->buf.size());

        eventset(ev,fd,&WorkerReactor::senddata,ev);    //设置该fd对应的回调函数为senddata
        eventadd(EPOLLOUT, ev);         //将fd加入红黑树中，监听其写事件

    } else if(len == 0){//对端已关闭
        close(ev->fd);
        printf("[fd = %d] pos[%ld], closed\n", fd, ev-r_events);
    }else{
        close(ev->fd);
        printf("recv[fd = %d] str.size() == [%d] error[%d]:%s\n",fd,len,errno,strerror(errno));
    }

    pthread_mutex_unlock(&event_mutex); // 解锁

}


//初始化事件
void WorkerReactor::eventset(event * ev, int fd, void (WorkerReactor::* call_back)(int ,int , void *), void * arg){
    ev -> fd = fd;
    fd_write_mutexes[fd] = std::make_unique<std::mutex>();
    fd_read_mutexes[fd] = std::make_unique<std::mutex>();

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
void WorkerReactor::eventadd(int events, event * ev){
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
void WorkerReactor::InitListenSocket(unsigned short port){
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
    //addr.sin_addr.s_addr = INADDR_ANY;    //绑定网卡上任意ip
    inet_pton(AF_INET, server_ip_.c_str(), &addr.sin_addr); //连接指定端口

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    bind(lfd,(sockaddr *)&addr, sizeof addr);

    listen(lfd, 100);

    eventset(&r_events[MAX_EVENTS], lfd, &WorkerReactor::acceptconn, &r_events[MAX_EVENTS]);
    eventadd(EPOLLIN, &r_events[MAX_EVENTS]);
}



void WorkerReactor::readctorinit(unsigned short port){
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
            
            long duration = now - r_events[chekckpos].last_active;   //计算客户端不活跃的时间
            if(duration >= 600){//超时，断连
                printf("[fd = %d] timeout\n", r_events[chekckpos].fd);
                pthread_mutex_lock(&event_mutex); // 加锁
                if(!datareactor){
                    std::string uid = socket_to_uid[r_events[chekckpos].fd];
                    handler hd({}, r_events[chekckpos].fd);
                    hd.uulg(uid);
                }
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

WorkerReactor::WorkerReactor(unsigned short port, pthread_pool& sharedPool, bool datarct)
                            : pthpool(sharedPool), datareactor(datarct) {
}
 

WorkerReactor::~WorkerReactor() {
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