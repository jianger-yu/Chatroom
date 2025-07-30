#pragma once
#include "../MessageQueue.hpp"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>

std::atomic<bool> file_sending(false);

class filefucs{
private:
    user& u;
    void* clientp;
    int page = 0;
    FILE* file;
    char arr[1024];
    char path[3072];
    std::string fid;
    std::string sd;
    Client dataclient;

public:

    filefucs(user& arg1, void*p):u(arg1),clientp(p){
    };
    //发送文件线程
    void upload_file_with_offset();
    //连接上服务器的文件传输线程
    bool conntect_filepth();
    //获取文件名
    std::string GetFileName(const char arr[]);
    //处理发送文件的逻辑
    void sendfile_touser(char);
    //列好友表
    void list(char);
    void listfriend();
};

std::string filefucs::GetFileName(const char arr[]){
  int i = strlen(arr) - 1;
  std::string str;
  for(;i >= 0; i--){
    if(arr[i] == '/') break;
    else str.push_back(arr[i]);
  }
  std::reverse(str.begin(), str.end());
  return str;
}

bool filefucs::conntect_filepth(){
    return dataclient.connectToHost("127.0.0.1", 5513);
    Socket* sock = dataclient.getSocket();
    sock->setNonBlocking();
}


void filefucs::sendfile_touser(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    if(file_sending){
        printf("\033[0;31m当前还有文件未传输完，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        charget();
        return ;
    }

    //找到对应消息
    int i = 5*page + c - '0' - 1, j = 0;
    if(i >= u.friendlist.size()) return;
    for(std::string str : u.friendlist){
        if(j == i){
            sd = str;
            break;
        }
        j++;
    }

    std::string str = "STOR";
    int ret = sock->sendMsg(str);
    if(ret == -1){
        printf("send 'STOR' command error:%s\n",strerror(errno));
        exit(1);
    }
    str.clear();
    //下载文件
    //输出指引
    printf("\033[0;32m输入需要传输的文件路径:>\033[0m");
    while(1){
        fflush(stdout); // 手动刷新标准输出缓冲区

        memset(arr,0,sizeof arr);
        //获取输入文件名
        ret = read(STDIN_FILENO,arr,sizeof arr);
        printf("输入路径为：%s",arr);
        if(ret <= 0){
            printf("输入错误:%s\n",strerror(errno));
            exit(1);
        }
        if(arr[0] == '\n') {
            system("clear");
            printf("\033[0;32m输入包含ESC的语句可回到上一级页面。\n路径有误,请重新输入:>\033[0m");
            continue;
        }
        arr[ret - 1] = '\0';
        char *pa = NULL;
        pa = getcwd(NULL, 0);
        //若不为绝对路径，拼接路径
        if(arr[0]!='/')
            sprintf(path,("%s/%s"),pa,arr);
        //若为绝对路径，直接传入路径
        else strcpy(path,arr);
        file = fopen(path, "rb");
        if (file == NULL) {
            system("clear");
            for(int i = 0; i < strlen(arr); i++) if(arr[i] == 27) return;
            printf("\033[0;32m输入包含ESC的语句可回到上一级页面。\n路径有误,请重新输入:>\033[0m");
            continue;
        }
        else break;
    }
    printf("输入成功:%s\n",arr);
    printf("正在上传文件...\n");
    //获取fid
    sock->sendMsg("gtfi:"+u.uid+":"+path);
    fid = EchoMsgQueue.wait_and_pop();


    //获取数据传输套接字
    Socket* datasock = dataclient.getSocket();
    //发送文件名
    // str.clear();
    // str = GetFileName(arr);
    // datasock->sendMsg("sdfl:"+u.uid+":"+sd+":"+fid +":"+str);

    //获取文件状态
    struct stat st;
    if(lstat(path,&st) == -1){//获取文件状态
        perror("stat");//处理错误返回值
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        charget();
        return ;
    }
    std::thread sendfilepth(&filefucs::upload_file_with_offset, this);
    sendfilepth.detach();  //后台运行，不阻塞主线程

    printf("\033[0;31m请按任意键继续...\033[0m");
    charget();
    return;
}

void filefucs::upload_file_with_offset() {
    file_sending = true;
    Socket* datasock = dataclient.getSocket();
    const size_t block_size = 4096;
    char buf[block_size];
    off_t offset = 0;
    size_t bytesRead;
    int i = 0;
    file_block block;
    while ((bytesRead = fread(buf, 1, block_size, file)) > 0) {
        // 构造 file_block
        block.sender_uid = u.uid;
        block.receiver_uid = sd;
        block.fid = fid;
        block.filename = arr;
        block.timestamp = file_block::get_beijing_time();
        block.offset = offset;
        block.is_group = false;
        block.is_file = true;

        std::string json_str = block.toJson();
        uint32_t json_len = htonl(json_str.size());

        // 构造完整消息
        std::string packet;
        packet.append(reinterpret_cast<const char*>(&json_len), sizeof(json_len));
        packet.append(json_str);
        packet.append(buf, bytesRead);
        // printf("[%d]\njson_len:%d packet.size():%ld\njson_str:%s\n", ++i, json_len,packet.size(), json_str.c_str());
        // printf("\n");
        if (datasock->sendFILE("rvfl:"+packet) == -1) {
            printf("发送失败，连接异常\n");
            fclose(file);
            file_sending = false;
            return;
        }

        offset += bytesRead;
    }

    fclose(file);
    datasock->sendMsg("fled:"+block.toJson());  // 通知结束
    file_sending = false;
}

void filefucs::list(char c){
    reportfucs rpf(u, clientp);
    bool ret = rpf.Getrpt();
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    if(!u.friendlist.size()){
        printf("\033[0;32m当前没有好友。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    int cnt = 0;
    cnt = u.friendlist.size();
    int maxpage = cnt / 5, i = 0;
    if(cnt % 5 != 0) maxpage++;
    if(c == '[' && page == 0) ;
    else if(c == '[') page --;
    if(c == ']' && page+1 >= maxpage) ;
    else if(c == ']') page ++;
    printf("\033[0;36m==========================================================\033[0m\n");
    reportfucs::newreport(u, clientp);
    printf("\033[0;32m以下为您的好友\033[0m\n");
    printf("\033[0;34m%-6s %-15s %-13s %-12s\033[0m\n", "序号", "用户名", "UID", "在线状态");
    for(std::string str : u.friendlist){
        if(i >= 5*page && i < 5*(page+1)){
            sock->sendMsg("gtus:"+str);
            std::string red = EchoMsgQueue.wait_and_pop();
            user ud = user::fromJson(red);
            std::string name = ud.name, status;
            if(ud.stat == "online") status = "在线";
            else if(ud.stat == "offline") status = "离线";
            else if(ud.stat == "deleted") status = "该账户已注销";

            // 如果是在线，颜色绿色；否则灰色
            const char *color = (status == "在线") ? "\033[0;32m" : "\033[0;90m";

            printf("%s[%d]  %-12s %-14s %-12s\033[0m",
                    color, i - 5 * page + 1,
                    name.c_str(), ud.uid.c_str(), status.c_str());
            if(rpf.rpt.chatfriend[ud.uid]) printf("   \033[0;31m（%d）\033[0m\n", rpf.rpt.chatfriend[ud.uid]);
            else puts("");
        }
        i++;
    }
    printf("                     \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                \033[0;32m[%d/%d]\033[0m\n",page+1,maxpage);
    printf("\033[0;36m==========================================================\033[0m\n");
}

void filefucs::listfriend(){
    system("clear");
    page = 0;
    list('0');
    if(u.friendlist.size())
        printf("\033[0;32m选择您要传文件的好友:>\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false, sendfileok = false;
    std::string msg;
    while(1){
        //判断用户信息是否变动
        if(UserMsgQueue.try_pop(msg)){
            page = 0;
            u = user::fromJson(msg);
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg) || flag || sendfileok){
            if(sendfileok){
                dataclient.reinitialize();
                conntect_filepth();
                sendfileok = false;
            }
            flag = false;
            system("clear");
            list('p');
            if(u.friendlist.size())
                printf("\033[0;32m选择您要传文件的好友:>\033[0m");
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        char input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            int p = 5*page + input - '0' - 1;
            if(p >= 0 && p < u.friendlist.size()){
                sendfile_touser(input);
                flag = true;
                sendfileok = true;
            }
            break;
        }
        case '[':{
            system("clear");
            list('[');
            if(u.friendlist.size())
                printf("\033[0;32m选择您要传文件的好友:>\033[0m");
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
            if(u.friendlist.size())
                printf("\033[0;32m选择您要传文件的好友:>\033[0m");
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case 27:{
            return ;
        }
        default:continue;
        }
    }
    return ;
}