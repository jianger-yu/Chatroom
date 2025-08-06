#pragma once
#include<iostream>
#include <fcntl.h>
#include "../../client.h"
#include "../../../user.hpp"
#include <unistd.h>
#include <cstring>
#include "../MessageQueue.hpp"
#include <chrono>

extern user us;
extern report rpt;

// 全局变量
std::chrono::steady_clock::time_point start_time;
void startTime() {
    start_time = std::chrono::steady_clock::now();
}
int throughtime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return static_cast<int>(duration.count());
}


class reportfucs{
private:
    void* clientp;
    int page = 0;

    int ctpage = 0;
    int msgcnt = 0;
    messages save;
    group viewgp;
public:

    reportfucs(user& arg1, void*p):clientp(p){
    };
    reportfucs(){};
    void reportinit(){
    };
    //获取当前全部通知
    //（外部函数）

    //处理好友申请页面
    void friendreport();
    void groupreport();
    //好友申请页面的菜单
    void menu(char c , int fg);
    //处理好友申请的函数
    void handleapply(char );
    //处理加群申请
    void handlegroupapply(char );
    //查看事务通知的页面
    void notice();
    //分析公告
    void Analysisnotice(std::string&str, int i);
    //处理公告
    void handlenotice(char );
    //查看聊天请求的页面
    void ctfrdreport();
    //查看群聊天请求的页面
    void ctfgrpreport();
    //处理聊天
    void handlechat(char, report& rpt);
    //聊天界面
    void chatmenu(char c, user& ud2);

    void gchatmenu(char c, group& gp);
    void ghandlechat(char c, int fg);

    //打印新通知
    static void newreport(user&us, void* p){

        if(rpt.friendapply.size())
        printf("           \033[0;34m[new] 有%ld条未处理的好友申请\033[0m\n", rpt.friendapply.size());
            if(rpt.total_friend_msg)
        printf("           \033[0;34m[new] 有%d个未读的好友消息\033[0m\n", rpt.total_friend_msg);
            if(rpt.total_group_msg)
        printf("           \033[0;34m[new] 有%d个未读的群聊消息\033[0m\n", rpt.total_group_msg);
            if(rpt.groupapply.size())
        printf("           \033[0;34m[new] 有%ld条未处理的加群申请\033[0m\n", rpt.groupapply.size());
        int cnt = 0;
        for(int i = 0; i < rpt.notice.size(); i++)
            if(rpt.notice[i][5] == 'n') cnt++;
        if(cnt)
        printf("           \033[0;34m[new] 有%d条未读的通知\033[0m\n", cnt);
    }
};

bool Getrpt(void *clientp){
    Client * cp = (Client*)clientp;
    Socket* sock = cp->getSocket();
    sock->sendMsg("gtrp:"+us.uid);
    //读取json字符串
    std::string js;
    js = EchoMsgQueue.wait_and_pop();
    if(js == "none") return false;
    rpt = report::fromJson(js);
    
    return true;
}

void reportfucs::Analysisnotice(std::string &str, int i){
    if(str[0] == 'a' && str[1] == 'd' && str[2] == 'f'){
        std::string result;
        bool yellow = false;
        if(str[3] == 'y') result = "同意";
        else result = "拒绝";
        if(str[5] == 'n') yellow = true;
        str.erase(str.begin(), str.begin() + 7);
        if(i == -1){
            printf("用户 \033[0;34m%s\033[0m %s了您的好友申请\n", str.c_str(), result.c_str());
            return;    
        }
        if(!yellow) 
            printf("[%d] %d、用户 \033[0;34m%s\033[0m %s了您的好友申请\n", i-5*page+1, i + 1, str.c_str(), result.c_str());
        else
            printf("\033[0;33m[%d] %d、用户\033[0m \033[0;34m%s\033[0m \033[0;33m%s了您的好友申请\033[0m\n", i-5*page+1, i + 1, str.c_str(), result.c_str());
    }
    else if(str[0] == 'a' && str[1] == 'd' && str[2] == 'g'){
        std::string result, uname, gname, handname;
        bool yellow = false;
        if(str[3] == 'y' || str[3] == 'Y') result = "同意";
        else result = "拒绝";
        if(str[5] == 'n' || str[3] == 'N') yellow = true;
        //解析命令"adg%c(n)%s:%s:%s", input, uname.c_str(), gname.c_str(), handname.c_str()
        int j = 0;
        while(str[j] != ')') j++;
        j++;
        while(str[j] != ':') uname.push_back(str[j++]);
        j++;
        while(str[j] != ':') gname.push_back(str[j++]);
        handname = str.c_str() + j + 1;
        if(i == -1){
            printf("用户 \033[0;34m%s\033[0m 加入群聊 \033[0;34m%s\033[0m 的申请被 \033[0;34m%s\033[0m。\n处理人：\033[0;34m%s\n\033[0m", uname.c_str(), gname.c_str(), result.c_str(), handname.c_str());
            return;    
        }
        if(!yellow) 
            printf("[%d] %d、用户 \033[0;34m%s\033[0m 加入群聊 \033[0;34m%s\033[0m 的申请被 \033[0;34m%s\033[0m。\n", i-5*page+1, i + 1,  uname.c_str(), gname.c_str(), result.c_str());
        else
            printf("\033[0;33m[%d] %d、用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 加入群聊 \033[0m\033[0;34m%s\033[0m \033[0;33m的申请被\033[0m \033[0;34m%s\033[0m\033[0;33m。\033[0m\n", i-5*page+1, i + 1,  uname.c_str(), gname.c_str(), result.c_str());
    }
    else if(str[0] == 'a' && str[1] == 'd' && str[2] == 'm'){
        std::string result = "解除", gname, uname;
        if(str[3] == 'y' || str[3] == 'Y') result = "设为";
        bool yellow = false;
        if(str[5] == 'n' || str[3] == 'N') yellow = true;
        //解析命令"admy(n)%s:%s", gp.name.c_str(), ud.name.c_str()
        int j = 0;
        while(str[j] != ')') j++;
        j++;
        while(str[j] != ':') gname.push_back(str[j++]);
        uname = str.c_str() + j + 1;
        if(i == -1){
            printf("用户 \033[0;34m%s\033[0m 在群聊 \033[0;34m%s\033[0m 被群主%s管理员。 \n", uname.c_str(), gname.c_str(), result.c_str());
            return;    
        }
        if(!yellow) 
            printf("[%d] %d、用户 \033[0;34m%s\033[0m 在群聊 \033[0;34m%s\033[0m 被群主%s管理员。\n", i-5*page+1, i + 1,  uname.c_str(), gname.c_str(), result.c_str());
        else
            printf("\033[0;33m[%d] %d、用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 在群聊 \033[0m\033[0;34m%s\033[0m \033[0;33m被群主%s管理员。\033[0m\n", i-5*page+1, i + 1,  uname.c_str(), gname.c_str(), result.c_str());
    }
    else if(str[0] == 'k' && str[1] == 'c' && str[2] == 'm'){
        std::string result, uname, gname, handname;
        bool yellow = false;
        if(str[5] == 'n' || str[3] == 'N') yellow = true;
        //解析命令"kcmb(n)%s:%s:%s", input, uname.c_str(), gname.c_str(), handname.c_str()
        int j = 0;
        while(str[j] != ')') j++;
        j++;
        while(str[j] != ':') uname.push_back(str[j++]);
        j++;
        while(str[j] != ':') gname.push_back(str[j++]);
        handname = str.c_str() + j + 1;
        if(i == -1){
            printf("用户 \033[0;34m%s\033[0m 被踢出了群聊 \033[0;34m%s\033[0m 。\n处理人：\033[0;34m%s\n\033[0m", uname.c_str(), gname.c_str(), handname.c_str());
            return;    
        }
        if(!yellow) 
            printf("[%d] %d、用户 \033[0;34m%s\033[0m 被踢出了群聊\033[0;34m%s\033[0m 。\n", i-5*page+1, i + 1,  uname.c_str(), gname.c_str());
        else
            printf("\033[0;33m[%d] %d、用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 被踢出了群聊 \033[0m\033[0;34m%s\033[0m 。\033[0m\n", i-5*page+1, i + 1,  uname.c_str(), gname.c_str());
    }
    else if(str[0] == 'd' && str[1] == 'i' && str[2] == 's'){
        std::string gname;
        bool yellow = false;
        if(str[5] == 'n' || str[3] == 'N') yellow = true;
        //解析命令"disg(n)%s", gname.c_str()
        int j = 0;
        while(str[j] != ')') j++;
        j++;
        for(;j < str.size(); j++) gname.push_back(str[j]);
        if(i == -1){
            printf("群聊 \033[0;34m%s\033[0m 被群主解散了。\033[0;34m\n", gname.c_str());
            return;    
        }
        if(!yellow) 
            printf("[%d] %d、群聊 \033[0;34m%s\033[0m  被群主解散了。\n", i-5*page+1, i + 1, gname.c_str());
        else
            printf("\033[0;33m[%d] %d、群聊 \033[0m\033[0;34m%s\033[0m\033[0;33m  被群主解散了。\033[0m\033[0m\n", i-5*page+1, i + 1,  gname.c_str());
    }
    else if(str[0] == 's' && str[1] == 'd' && str[2] == 'f'){
        std::string result, uname, recvname, filename;
        bool yellow = false;
        if(str[5] == 'n' || str[3] == 'N') yellow = true;
        //解析命令"sdfu(n)%s:%s:%s", uname.c_str(), recvname.c_str(), filename.c_str()
        //解析命令"sdfg(n)%s:%s:%s", uname.c_str(), recvgroupname.c_str(), gfilename.c_str()
        int j = 0;
        while(str[j] != ')') j++;
        j++;
        while(str[j] != ':') uname.push_back(str[j++]);
        j++;
        while(str[j] != ':') recvname.push_back(str[j++]);
        filename = str.c_str() + j + 1;
        if(us.name == uname){
            if(i == -1){
                if(str[3] == 'u')
                    printf("您对用户 \033[0;34m%s\033[0m 发送文件 \033[0;34m%s\033[0m成功。\n", recvname.c_str(), filename.c_str());
                else
                    printf("您对群聊 \033[0;34m%s\033[0m 发送群文件 \033[0;34m%s\033[0m成功。\n", recvname.c_str(), filename.c_str());
                return;    
            }
            if(!yellow){
                if(str[3] == 'u')
                    printf("[%d] %d、您对用户 \033[0;34m%s\033[0m 发送文件 \033[0;34m%s\033[0m成功。\n", i-5*page+1, i + 1, recvname.c_str(), filename.c_str());
                else
                    printf("[%d] %d、您对群聊 \033[0;34m%s\033[0m 发送群文件 \033[0;34m%s\033[0m成功。\n", i-5*page+1, i + 1, recvname.c_str(), filename.c_str());
            }
            else{
                if(str[3] == 'u')
                    printf("\033[0;33m[%d] %d、您对用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 发送文件 \033[0m\033[0;34m%s\033[0m \033[0;33m成功。\033[0m\n", i-5*page+1, i + 1, recvname.c_str(), filename.c_str());
                else
                    printf("\033[0;33m[%d] %d、您对群聊 \033[0m\033[0;34m%s\033[0m\033[0;33m 发送群文件 \033[0m\033[0;34m%s\033[0m \033[0;33m成功。\033[0m\n", i-5*page+1, i + 1, recvname.c_str(), filename.c_str());
            }
            return;
        }
        if(i == -1){
            if(str[3] == 'u')
                printf("用户 \033[0;34m%s\033[0m 对您发送了文件 \033[0;34m%s\033[0m 。\n", uname.c_str(), filename.c_str());
            else 
                printf("用户 \033[0;34m%s\033[0m 在群聊 \033[0;34m%s\033[0m 发送了群文件 \033[0;34m%s\033[0m 。\n", uname.c_str(), recvname.c_str(), filename.c_str());
            return;    
        }
        if(!yellow){
            if(str[3] == 'u')
                printf("[%d] %d、用户 \033[0;34m%s\033[0m 对您发送了文件 \033[0;34m%s\033[0m 。\n", i-5*page+1, i + 1,  uname.c_str(), filename.c_str());
            else 
                printf("[%d] %d、用户 \033[0;34m%s\033[0m 在群聊 \033[0;34m%s\033[0m 发送了群文件 \033[0;34m%s\033[0m 。\n", i-5*page+1, i + 1,  uname.c_str(), recvname.c_str(), filename.c_str());
        }
        else{
            if(str[3] == 'u')
                printf("\033[0;33m[%d] %d、用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 对您发送了文件 \033[0m\033[0;34m%s\033[0m \033[0;33m。\033[0m\n", i-5*page+1, i + 1,  uname.c_str(), filename.c_str());
            else 
                printf("\033[0;33m[%d] %d、用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 在群聊 \033[0m\033[0;34m%s\033[0m\033[0;33m 发送了群文件 \033[0m\033[0;34m%s\033[0m \033[0;33m。\033[0m\n", i-5*page+1, i + 1,  uname.c_str(), recvname.c_str(), filename.c_str());
        }
    }
    else if(str[0] == 'r' && str[1] == 'v' && str[2] == 'f'){
        //解析命令"rvfu(n)%s:%s:%s", uname.c_str(), sendname.c_str(), filename.c_str()
        //解析命令"rvfg(n)%s:%s:%s", uname.c_str(), recvgroupname.c_str(), gfilename.c_str()
         std::string result, uname, sendname, filename;
        bool yellow = false;
        if(str[5] == 'n' || str[3] == 'N') yellow = true;
        int j = 0;
        while(str[j] != ')') j++;
        j++;
        while(str[j] != ':') uname.push_back(str[j++]);
        j++;
        while(str[j] != ':') sendname.push_back(str[j++]);
        filename = str.c_str() + j + 1;
        if(i == -1){
            if(str[3] == 'u')
                printf("您成功接收了用户 \033[0;34m%s\033[0m 发送的文件 \033[0;34m%s\033[0m。\n", sendname.c_str(), filename.c_str());
            else
                printf("您成功接收了群聊 \033[0;34m%s\033[0m 的群文件 \033[0;34m%s\033[0m。\n", sendname.c_str(), filename.c_str());
            return;    
        }
        if(!yellow){
            if(str[3] == 'u')
                printf("[%d] %d、您成功接收了用户 \033[0;34m%s\033[0m 发送的文件 \033[0;34m%s\033[0m。\n", i-5*page+1, i + 1, sendname.c_str(), filename.c_str());
            else
                printf("[%d] %d、您成功接收了群聊 \033[0;34m%s\033[0m 的群文件 \033[0;34m%s\033[0m。\n", i-5*page+1, i + 1, sendname.c_str(), filename.c_str());
        }
        else{
            if(str[3] == 'u')
                printf("\033[0;33m[%d] %d、您成功接收了用户 \033[0m\033[0;34m%s\033[0m\033[0;33m 发送的文件 \033[0m\033[0;34m%s\033[0m \033[0;33m。\033[0m\n", i-5*page+1, i + 1, sendname.c_str(), filename.c_str());
            else
                printf("\033[0;33m[%d] %d、您成功接收了群聊 \033[0m\033[0;34m%s\033[0m\033[0;33m 的群文件 \033[0m\033[0;34m%s\033[0m \033[0;33m。\033[0m\n", i-5*page+1, i + 1, sendname.c_str(), filename.c_str());
        }
        return;
    }
}


void reportfucs::menu(char c, int fg){
    if(fg == 1 && rpt.friendapply.size() == 0){
        printf("\033[0;32m当前没有未处理的好友申请。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    } 
    else if(fg == 2 && rpt.total_friend_msg == 0){
        printf("\033[0;32m当前没有未读的好友消息。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    else if(fg == 3 && rpt.total_group_msg == 0){
        printf("\033[0;32m当前没有未读的群聊消息。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    else if(fg == 4 && rpt.groupapply.size() == 0){
        printf("\033[0;32m当前没有加群申请。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    else if(fg == 5 && rpt.notice.size() == 0){
        printf("\033[0;32m当前没有事务通知。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    int cnt = 0;
    if(fg == 1)
        cnt = rpt.friendapply.size();
    else if(fg == 2){
        for(auto m : rpt.chatfriend)
        if(m.second) cnt++;
    }
    else if(fg == 3){
        for(auto m : rpt.chatgroup)
        if(m.second) cnt++;
    }
    else if(fg == 4)
        cnt = rpt.groupapply.size();
    else if(fg == 5)
        cnt = rpt.notice.size();
    int maxpage = cnt / 5, i = 0;
    if(cnt % 5 != 0) maxpage++;
    if(c == '[' && page == 0) ;
    else if(c == '[') page --;
    if(c == ']' && page+1 >= maxpage) ;
    else if(c == ']') page ++;
    printf("\033[0;36m==========================================================\033[0m\n");
    if(fg == 1)
        printf("\033[0;32m以下为您未处理的好友申请\033[0m\n");
    else if(fg == 2)
        printf("\033[0;32m以下为您有未读消息的好友\033[0m\n");  
    else if(fg == 3)
        printf("\033[0;32m以下为您有未读消息的群聊\033[0m\n");
    else if(fg == 4)
        printf("\033[0;32m以下为未被处理的加群申请\033[0m\n");    
    else if(fg == 5)
        printf("\033[0;32m以下为您收到的通知,黄色为未读部分\033[0m\n");
    if(fg == 1){
        for(std::string str : rpt.friendapply){
            if(i >= 5*page && i < 5*(page+1)){
                printf("\033[0;32m[%d] %d、用户\033[0m \033[0;34m%s\033[0m \033[0;32m请求添加您为好友\033[0m\n", i-5*page+1, i + 1, str.c_str());
            }
            i++;
        }
    } 
    else if (fg == 2){
        Client* cp = (Client*)clientp;
        Socket * sock = cp->getSocket(); 
        printf("\033[0;34m%-6s %-15s %-13s %-12s\033[0m\n", "序号", "用户名", "UID", "在线状态");
        for(auto str : rpt.chatfriend){
            if(str.second == 0) continue;
            if(i >= 5*page && i < 5*(page+1)){
                sock->sendMsg("gtus:"+str.first);
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
                if(rpt.chatfriend[ud.uid]) printf("   \033[0;31m（%d）\033[0m\n", rpt.chatfriend[ud.uid]);
                else puts("");
            }
            i++;
        }
    }
    else if(fg == 3){
        Client* cp = (Client*)clientp;
        Socket * sock = cp->getSocket(); 
        printf("\033[0;34m%-6s %-14s %-13s %-12s\033[0m\n", "序号", "群名", "GID", "在群中身份");
        for(auto str : rpt.chatgroup){
            if(str.second == 0) continue;
            if(i >= 5*page && i < 5*(page+1)){
                sock->sendMsg("gtgp:"+str.first);
                std::string red = EchoMsgQueue.wait_and_pop();
                if(red == "norepeat") continue;
                group ud = group::fromJson(red);
                std::string name = ud.name, status = "成员";
                const char *color = "\033[0;37m";
                if(ud.owner == us.uid) {
                    status = "群主";
                    color = "\033[0;31m";
                } else{
                    for(std::string str : ud.managelist){
                        if(str == us.uid){
                            status = "管理员";
                            color = "\033[0;32m";
                        }
                    }
                }
                printf("\033[0;37m[%d]  %-12s %-14s \033[0m%s%-12s\033[0m",
                        i - 5 * page + 1,
                        name.c_str(), ud.gid.c_str(), color,status.c_str());
                if(rpt.chatgroup[ud.gid]) printf("   \033[0;31m（%d）\033[0m\n", rpt.chatgroup[ud.gid]);
                else puts("");
            }
            i++;
        }
    } else if(fg == 4){
        for(std::string str : rpt.groupapply){
            if(i >= 5*page && i < 5*(page+1)){
                std::string uname;
                int j = 0;
                while(str[j] != ':') uname.push_back(str[j++]);
                printf("\033[0;32m[%d] %d、用户\033[0m \033[0;34m%s\033[0m \033[0;32m申请加入群聊\033[0m \033[0;33m%s\033[0m\n", i-5*page+1, i + 1, uname.c_str(), str.c_str()+j+1);
            }
            i++;
        }
    }
    else if (fg == 5){
        for(std::string str : rpt.notice){
            if(i >= 5*page && i < 5*(page+1)){
                Analysisnotice(str, i);
            }
            i++;
        }
    }
    printf("                     \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                \033[0;32m[%d/%d]\033[0m\n",page+1,maxpage);

    printf("\033[0;36m==========================================================\033[0m\n");
    printf("\033[0;32m请输入您要处理的通知前的序号:>\033[0m");
}

void reportfucs::handleapply(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    fflush(stdout);
    int i = 0;
    std::string name, sd, rev;
    for(std::string str : rpt.friendapply)
        if((i++ + 1) == 5*page + c -'0')
            name = str;
    if(!name.size()) return;
    printf("\033[0;32m是否同意用户\033[0m \033[0;34m%s\033[0m \033[0;32m的好友申请？（Y/N）\033[0m\n", name.c_str());
    char input = 0;
    sd = "adf(";
    while(1){
        input = charget();
        if(input == 27) return;
        if(input != 'Y' && input != 'N' && input != 'y' && input != 'n') continue;
        if(input == 'Y' || input == 'y') sd.push_back('y');
        else sd.push_back('n');
        break;
    }
    //改本地
    if(input == 'Y' || input == 'y'){
        sock->sendMsg("gtud:"+name);
        rev = EchoMsgQueue.wait_and_pop();
        if(rev == "false"){
            printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
            printf("\033[0;31m请按任意键继续...\033[0m");
            input = charget();
            return ;
        }
        us.friendlist.insert(rev);
    }
    //xxx:user1.name:user2:uid(用户1加用户2),改数据库
    sock->sendMsg(sd+":"+name+":"+us.uid);
    sd = EchoMsgQueue.wait_and_pop();
    if(sd == "right"){
        if(input == 'Y' || input == 'y')
            printf("\033[0;32m添加好友成功！\033[0m\n");
        else 
            printf("\033[0;32m已拒绝该申请！\033[0m\n");
        printf("\033[0;32m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }  else{
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
}

void reportfucs::handlegroupapply(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    fflush(stdout);
    int i = 0, j = 0;
    std::string rptstr, sd, rev, name, gname;
    for(std::string str : rpt.groupapply)
        if((i++ + 1) == 5*page + c -'0')
            rptstr = str;
    if(!rptstr.size()) return;
    while(rptstr[j] != ':') name.push_back(rptstr[j++]);
    gname = rptstr.c_str() + j + 1;
    printf("\033[0;32m是否同意用户\033[0m \033[0;34m%s\033[0m \033[0;32m加入群聊\033[0m \033[0;33m%s\033[0m \033[0;32m？（Y/N）\033[0m\n", name.c_str(), gname.c_str());
    char input = 0;
    sd = "adg(";
    while(1){
        input = charget();
        if(input == 27) return;
        if(input != 'Y' && input != 'N' && input != 'y' && input != 'n') continue;
        if(input == 'Y' || input == 'y') sd.push_back('y');
        else sd.push_back('n');
        break;
    }

    
    //adg(y:uname:gname:handler(用户加群),改数据库
    sock->sendMsg(sd+":"+name+":"+gname+":"+us.name);
    sd = EchoMsgQueue.wait_and_pop();
    if(sd == "ingrp"){
        printf("\033[0;31m该申请已被处理。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        charget();
        return ;
    } else if(sd == "disgp"){
        printf("\033[0;31m该群聊已解散。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        charget();
        return ;
    }
    if(sd == "right"){
        if(input == 'Y' || input == 'y')
            printf("\033[0;32m已经同意该申请！\033[0m\n");
        else 
            printf("\033[0;32m已拒绝该申请！\033[0m\n");
        printf("\033[0;32m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }  else{
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
}



void reportfucs::friendreport(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0', 1);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            Getrpt(clientp);
            ReptMsgQueue.clear();
            flag = true;
        }
        if(flag){
            flag = false;
            system("clear");
            if(!Getrpt(clientp)) {
                printf("\033[0;31m数据异常，请稍后再试\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m");
                input = charget();
                return;
            }
            menu('p', 1);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        
        input = tm_charget(1000);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            handleapply(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[', 1);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 1);
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

void reportfucs::groupreport(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0', 4);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            Getrpt(clientp);
            ReptMsgQueue.clear();
            flag = true;
        }
        if(flag){
            flag = false;
            system("clear");
            if(!Getrpt(clientp)) {
                printf("\033[0;31m数据异常，请稍后再试\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m");
                input = charget();
                return;
            }
            menu('p', 4);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(1000);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            handlegroupapply(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[', 4);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 4);
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



void reportfucs::handlenotice(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    fflush(stdout);
    //找到对应消息
    int i = 5*page + c - '0' - 1;
    if(i >= rpt.notice.size()) return;
    std::string str = rpt.notice[i], sd;
    //修改rpt
    rpt.notice[i][5] = 'y';
    sock->sendMsg("rdnt:"+us.uid+":"+str);
    sd = EchoMsgQueue.wait_and_pop();
    if(sd == "false"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        return;
    }

    printf("\033[0;32mNOTICE:\n\033[0m");
    Analysisnotice(str, -1);
    printf("\033[0;32m按r可删除这条通知,按ESC可返回上一级页面。\033[0m");
    fflush(stdout);
    while(1){
        char input = 0;
        input = charget();
        if(input == 27) return;
        else if(input == 'r' || input == 'R'){
            sock->sendMsg("rmnt:"+us.uid+":"+rpt.notice[i]);
            rpt.notice.erase(rpt.notice.begin() + i);
            sd = EchoMsgQueue.wait_and_pop();
            if(sd == "false"){
                printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
                return;
            }
            return;
        } else continue;
    }
}

void reportfucs::notice(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0', 5);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            Getrpt(clientp);
            ReptMsgQueue.clear();
            flag = true;
        }
        if(flag){
            flag = false;
            system("clear");
            fflush(stdout); // 手动刷新标准输出缓冲区
            menu('p', 5);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(1000);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            handlenotice(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[', 5);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 5);
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

void reportfucs::chatmenu(char c, user& ud2){
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    if( rpt.chatfriend[ud2.uid]){
        rpt.total_friend_msg -= rpt.chatfriend[ud2.uid];
        rpt.chatfriend[ud2.uid] = 0;
        sock->sendMsg("svrp:"+us.uid+":"+rpt.toJson());
        std::string rev = EchoMsgQueue.wait_and_pop();
    }
    int cnt = 0;
    cnt = save.data.size();
    int maxctpage = msgcnt / 7;
    int i = 0;
    if(msgcnt % 7 != 0) maxctpage++;
    if(maxctpage == 0) maxctpage = 1;
    if(c == '[' && ctpage == 0) ;
    else if(c == '[') ctpage --;
    if(c == ']' && ctpage+1 >= maxctpage) ;
    else if(c == ']') ctpage ++;
    std::string sender;
    printf("\033[0;36m=============================聊天页面=============================\033[0m\n");
    reportfucs::newreport(us, clientp);
    if(ud2.stat == "online") printf("\033[0;32m                             %s (在线)\033[0m\n", ud2.name.c_str());
    else                     printf("\033[0;90m                             %s (离线)\033[0m\n", ud2.name.c_str());
    if(ctpage+1 == maxctpage)    printf("\033[0;90m                       ---聊天记录已到顶---\033[0m\n");
    for(i = std::min(7*(ctpage+1) - 1, (int)save.data.size() - 1); i >= 7*ctpage; i--){
        if(!save.data.size()) break;
        message msg = message::fromJson(save.data[i]);
        sender = ud2.name;
        if(msg.sender_uid == us.uid) sender = us.name;
        if(ud2.stat == "online" || sender == us.name) printf("\033[0;32m%s\033[0m \033[0;33m[%s]\033[0m\n", sender.c_str(), msg.timestamp.c_str());
        else                     printf("\033[0;90m%s\033[0m \033[0;33m[%s]\033[0m\n", sender.c_str(), msg.timestamp.c_str());
        printf("\033[0;32m>\033[0m%s\n", msg.content.c_str());
    }
    printf("                                         \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                                         \033[0;32m[%d/%d]\033[0m\n",ctpage+1,maxctpage);
    printf("\033[0;36m==================================================================\033[0m\n");
}
void reportfucs::handlechat(char c, report& rpt){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    printf("\033[0;32m数据请求中...\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    //找到对应uid
    int i = 5*page + c - '0' - 1, j = 0;
    if(i >= rpt.chatfriend.size()) return;
    std::string uid2 = "none";
    for(auto str : rpt.chatfriend){
        if(j == i && str.second != 0){
            uid2 = str.first;
            break;
        }
        if(str.second != 0)
            j++;
    }
    if(uid2 == "none") return;
    sock->sendMsg("gtus:"+ uid2);
    std::string js2 = EchoMsgQueue.wait_and_pop();
    user ud2 = user::fromJson(js2);
    //开始聊天
    //拉取历史记录
    //读取两页消息（一页7句消息）
    sock->sendMsg("ctms:"+us.uid+":"+uid2);
    std::string rev = EchoMsgQueue.wait_and_pop(), msg;
    save = messages::fromJson(rev);
    //读取聊天记录总条数
    sock->sendMsg("rdpg:"+ us.uid + ":" + uid2);
    rev = EchoMsgQueue.wait_and_pop();
    sscanf(rev.c_str(), "%d", &msgcnt);
    //定义部分变量
    bool flag = false;
    system("clear");
    ctpage = 0;
    chatmenu('0', ud2);
    printf("\033[0;32m请输入:>\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    std::string content, utf8_buf;
    ChatMsgQueue.clear();
    while(1){
        //判断用户信息是否变动
        if(UserMsgQueue.try_pop(msg)){
            page = 0;
            us = user::fromJson(msg);
            if(us.friendlist.count(uid2) == 0){//被删除了
                system("clear");
                printf("\033[0;31m您被对方删除了。\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m\n");
                charget();
                return;
            }
            flag = true;
        }
        //判断聊天消息是否有新
        if(ChatMsgQueue.try_pop(msg)){
            page = 0;
            message m = message::fromJson(msg);
            if(m.sender_uid == ud2.uid&& m.is_group == false) save.data.insert(save.data.begin(), msg);
            msgcnt++;
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg)){
            Getrpt(clientp);
            ReptMsgQueue.clear();
            flag = true;
        }
        if(flag){
            flag = false;
            system("clear");
            chatmenu('p', ud2);
            printf("\033[0;32m请输入:>\033[0m");
            printf("%s", content.c_str());
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        char input = tm_charget(1000);
        if(input == -1) continue;
        // 中文或其他 UTF-8 字符处理
        switch(input){
        case 127:
        case '\b':{
            if (content.empty()) continue;
            int i = content.size() - 1;
            // 向后回退找到一个 UTF-8 字符的起始字节
            int len = 1;
            while (i - len >= 0 && (content[i - len + 1] & 0xC0) == 0x80) {
                len++;
            }
            int char_start = i - len + 1;
            if (char_start < 0 || char_start >= (int)content.size()) continue;  // 安全边界
            std::string ch = content.substr(char_start, len);
            // 判断字符宽度
            int display_width = is_wide_char((const unsigned char *)ch.c_str()) ? 2 : 1;
            // 删除字符
            content.erase(char_start, len);
            // 回退显示
            for (int j = 0; j < display_width; ++j) {
                printf("\b \b");
            }
            fflush(stdout);
            break;
        }
        case '\n':{//发送消息
            if(content.size() == 0 || content == "\n") break;
            if(us.shieldlist.count(uid2)){
                system("clear");
                printf("\033[0;31m对方已被您屏蔽，发送失败！\033[0m\n");
                printf("\033[0;31m请解除屏蔽后再发送。\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m\n");
                charget();
                return;
            }
            content.push_back('\0');
            message sendm;
            sendm.sender_uid = us.uid;
            sendm.sender_name = us.name;
            sendm.receiver_uid = uid2;
            sendm.content = content;
            sendm.timestamp = message::get_beijing_time();
            sock->sendMsg("sdms:"+sendm.toJson());
            rev = EchoMsgQueue.wait_and_pop();
            if(rev == "rihgt");
            else if(rev == "nofrd"){
                system("clear");
                printf("\033[0;31m当前不是好友，发送失败！\033[0m\n]]");
                printf("\033[0;31m请按任意键继续...\033[0m\n");
                charget();
                return;
            }
            save.data.insert(save.data.begin(), sendm.toJson());
            flag = true;
            ctpage = 0;
            content.clear();
            msgcnt++;
            break;
        }
        case '[':{
            system("clear");
            chatmenu('[', ud2);
            printf("\033[0;32m请输入:>\033[0m");
            printf("%s", content.c_str());
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            //请求新页的消息
            if(save.data.size() < msgcnt){
                char tmp[512];
                sprintf(tmp, "ndms:%s:%s:%ld", us.uid.c_str(), ud2.uid.c_str(), save.data.size());
                sock->sendMsg(tmp);
                messages mgs = messages::fromJson(EchoMsgQueue.wait_and_pop());
                for(int i = 0; i < mgs.data.size(); i++)
                    save.data.push_back(mgs.data[i]);
            }
            chatmenu(']', ud2);
            printf("\033[0;32m请输入:>\033[0m");
            printf("%s", content.c_str());
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case 27:{
            return ;
        }
        default:{
             utf8_buf += input;
            
            int need_len = 1;
            unsigned char first = static_cast<unsigned char>(utf8_buf[0]);
            if ((first & 0x80) == 0x00) need_len = 1;
            else if ((first & 0xE0) == 0xC0) need_len = 2;
            else if ((first & 0xF0) == 0xE0) need_len = 3;
            else if ((first & 0xF8) == 0xF0) need_len = 4;
            else {
                // 非法字符
                utf8_buf.clear();
                continue;
            }

            while ((int)utf8_buf.size() < need_len) {
                utf8_buf += charget(); // 继续收集字节
            }

            // 拼完一个字符
            content += utf8_buf;
            printf("%s", utf8_buf.c_str());
            fflush(stdout);
            utf8_buf.clear();
            break;
        }
        }
    }
}


void reportfucs::ctfrdreport(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0', 2);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            Getrpt(clientp);
            ReptMsgQueue.clear();
            flag = true;
        }
        if(flag){
            flag = false;
            system("clear");
            if(!Getrpt(clientp)) {
                printf("\033[0;31m数据异常，请稍后再试\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m");
                input = charget();
                return;
            }
            menu('p', 2);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(1000);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            handlechat(input, rpt);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[', 2);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 2);
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

void reportfucs::gchatmenu(char c, group& gp){
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    if( rpt.chatgroup[gp.gid]){
        rpt.total_group_msg -= rpt.chatgroup[gp.gid];
        rpt.chatgroup[gp.gid] = 0;
        sock->sendMsg("svrp:"+us.uid+":"+rpt.toJson());
        std::string rev = EchoMsgQueue.wait_and_pop();
    }
    int cnt = 0;
    cnt = save.data.size();
    int maxctpage = msgcnt / 7;
    int i = 0;
    if(msgcnt % 7 != 0) maxctpage++;
    if(maxctpage == 0) maxctpage = 1;
    if(c == '[' && ctpage == 0) ;
    else if(c == '[') ctpage --;
    if(c == ']' && ctpage+1 >= maxctpage) ;
    else if(c == ']') ctpage ++;
    std::string sender;
    printf("\033[0;36m=============================群聊页面=============================\033[0m\n");
    reportfucs::newreport(us, clientp);
    printf("\033[0;32m                             %s (%ld)\033[0m\n", gp.name.c_str(), gp.memberlist.size() + gp.managelist.size() + 1);
    if(ctpage+1 == maxctpage)    printf("\033[0;90m                       ---聊天记录已到顶---\033[0m\n");
    for(i = std::min(7*(ctpage+1) - 1, (int)save.data.size() - 1); i >= 7*ctpage; i--){
        if(!save.data.size()) break;
        message msg = message::fromJson(save.data[i]);
        if(msg.sender_uid == us.uid) sender = us.name;
        printf("\033[0;32m%s\033[0m \033[0;33m[%s]\033[0m\n", msg.sender_name.c_str(), msg.timestamp.c_str());
        printf("\033[0;32m>\033[0m%s\n", msg.content.c_str());
    }
    printf("                                         \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                                         \033[0;32m[%d/%d]\033[0m\n",ctpage+1,maxctpage);
    printf("\033[0;36m==================================================================\033[0m\n");
}

void reportfucs::ghandlechat(char c, int fg){
    int i = 5*page + c - '0' - 1, j = 0;
    if(fg == 2){
        for(auto m : rpt.chatgroup)
            if(m.second > 0)
                j++;
        if(i >= j) return ;
        j = 0;
    }
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    printf("\033[0;32m数据请求中...\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    //找到对应gid
    if(fg == 1){
        if(i >= us.grouplist.size()) return;
    }
    else if(fg == 2)
        if(i >= rpt.chatgroup.size()) return;
    std::string gid;
    if(fg == 1){
        for(std::string str : us.grouplist){
            if(j == i){
                gid = str;
                break;
            }
            j++;
        }
    } 
    else if (fg == 2){
        for(auto str : rpt.chatgroup){
            if(j == i && str.second != 0){
                gid = str.first;
                break;
            }
            if(str.second != 0)
                j++;
        }
    }
    sock->sendMsg("gtgp:"+ gid);
    std::string js = EchoMsgQueue.wait_and_pop();
    if(js == "norepeat"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        charget();
        return ;
    }
    viewgp = group::fromJson(js);
    //开始聊天
    //拉取历史记录
    //读取两页消息（一页7句消息）
    save.data.clear();
    sock->sendMsg("gctm:"+gid);
    std::string rev = EchoMsgQueue.wait_and_pop(), msg;
    save = messages::fromJson(rev);
    //读取聊天记录总条数
    sock->sendMsg("rdgc:"+gid);
    rev = EchoMsgQueue.wait_and_pop();
    sscanf(rev.c_str(), "%d", &msgcnt);
    //定义部分变量
    bool flag = false;
    system("clear");
    ctpage = 0;
    gchatmenu('0', viewgp);
    printf("\033[0;32m请输入:>\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    std::string content, utf8_buf;
    ChatMsgQueue.clear();
    while(1){
        int tm = throughtime();
        if(tm >= 1000 || flag){
            //判断用户信息是否变动
            if(UserMsgQueue.try_pop(msg)){
                page = 0;
                us = user::fromJson(msg);
                if(us.grouplist.count(gid) == 0){//被删除了
                    system("clear");
                    printf("\033[0;31m您当前已不在该群聊。\033[0m\n");
                    printf("\033[0;31m请按任意键继续...\033[0m\n");
                    charget();
                    return;
                }
                flag = true;
            }
            //判断聊天消息是否有新
            if(ChatMsgQueue.try_pop(msg)){
                page = 0;
                message m = message::fromJson(msg);
                if(m.sender_uid != us.uid && m.receiver_uid == gid && m.is_group) save.data.insert(save.data.begin(), msg);
                msgcnt++;
                flag = true;
            }
            //判断是否有新通知
            if(ReptMsgQueue.try_pop(msg)){
                Getrpt(clientp);
                ReptMsgQueue.clear();
                flag = true;
            }
            if(flag){
                flag = false;
                system("clear");
                gchatmenu('p', viewgp);
                printf("\033[0;32m请输入:>\033[0m");
                printf("%s", content.c_str());
                fflush(stdout); // 手动刷新标准输出缓冲区
            }
            startTime();
        }
        char input = tm_charget(1000);
        if(input == -1) continue;
        // 中文或其他 UTF-8 字符处理
        switch(input){
        case 127:
        case '\b':{
            if (content.empty()) continue;
            int i = content.size() - 1;
            // 向后回退找到一个 UTF-8 字符的起始字节
            int len = 1;
            while (i - len >= 0 && (content[i - len + 1] & 0xC0) == 0x80) {
                len++;
            }
            int char_start = i - len + 1;
            if (char_start < 0 || char_start >= (int)content.size()) continue;  // 安全边界
            std::string ch = content.substr(char_start, len);
            // 判断字符宽度
            int display_width = is_wide_char((const unsigned char *)ch.c_str()) ? 2 : 1;
            // 删除字符
            content.erase(char_start, len);
            // 回退显示
            for (int j = 0; j < display_width; ++j) {
                printf("\b \b");
            }
            fflush(stdout);
            break;
        }
        case '\n':{//发送消息
            if(content.size() == 0 || content == "\n") break;
            content.push_back('\0');
            message sendm;
            sendm.sender_uid = us.uid;
            sendm.sender_name = us.name;
            sendm.receiver_uid = gid;
            sendm.content = content;
            sendm.timestamp = message::get_beijing_time();
            sendm.is_group = true;
            sock->sendMsg("sdgm:"+sendm.toJson());
            rev = EchoMsgQueue.wait_and_pop();
            if(rev == "rihgt");
            else if(rev == "nofrd"){
                system("clear");
                printf("\033[0;31m当前不在群聊，发送失败！\033[0m\n]]");
                printf("\033[0;31m请按任意键继续...\033[0m\n");
                charget();
                return;
            }
            save.data.insert(save.data.begin(), sendm.toJson());
            flag = true;
            ctpage = 0;
            content.clear();
            msgcnt++;
            break;
        }
        case '[':{
            system("clear");
            gchatmenu('[', viewgp);
            printf("\033[0;32m请输入:>\033[0m");
            printf("%s", content.c_str());
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            //请求新页的消息
            if(save.data.size() < msgcnt){
                char tmp[512];
                sprintf(tmp, "ndgm:%s:%ld", gid.c_str(), save.data.size());
                sock->sendMsg(tmp);
                messages mgs = messages::fromJson(EchoMsgQueue.wait_and_pop());
                for(int i = 0; i < mgs.data.size(); i++)
                    save.data.push_back(mgs.data[i]);
            }
            gchatmenu(']', viewgp);
            printf("\033[0;32m请输入:>\033[0m");
            printf("%s", content.c_str());
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case 27:{
            return ;
        }
        default:{
             utf8_buf += input;
            
            int need_len = 1;
            unsigned char first = static_cast<unsigned char>(utf8_buf[0]);
            if ((first & 0x80) == 0x00) need_len = 1;
            else if ((first & 0xE0) == 0xC0) need_len = 2;
            else if ((first & 0xF0) == 0xE0) need_len = 3;
            else if ((first & 0xF8) == 0xF0) need_len = 4;
            else {
                // 非法字符
                utf8_buf.clear();
                continue;
            }

            while ((int)utf8_buf.size() < need_len) {
                utf8_buf += charget(); // 继续收集字节
            }

            // 拼完一个字符
            content += utf8_buf;
            printf("%s", utf8_buf.c_str());
            fflush(stdout);
            utf8_buf.clear();
            break;
        }
        }
    }
}

void reportfucs::ctfgrpreport(){
    char input = 0;
    page = 0;
    std::string msg;
    system("clear");
    menu('0', 3);
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    while(1){
        if(ReptMsgQueue.try_pop(msg)){
            Getrpt(clientp);
            ReptMsgQueue.clear();
            flag = true;
        }
        if(flag){
            flag = false;
            system("clear");
            if(!Getrpt(clientp)) {
                printf("\033[0;31m数据异常，请稍后再试\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m");
                input = charget();
                return;
            }
            menu('p', 3);
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        input = tm_charget(1000);
        if(input == -1) continue;
        switch(input){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':{
            ghandlechat(input, 2);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            menu('[', 3);
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            menu(']', 3);
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

reportfucs rpf;
report rpt;


