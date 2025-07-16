#pragma once
#include "../MessageQueue.hpp"
#include "reportfuc.hpp"
#include "../../../message.hpp"

class friendfucs{
private:
    user &u;
    void* clientp;
    int page = 0;
    int ctpage = 0;
    messages save;
public:

    friendfucs(user &arg1, void*p):u(arg1),clientp(p){
    };
    //仅展示好友列表
    void list(char);
    //添加好友
    void addfriend();
    //选定删除好友
    void delfriend();
    //处理删除功能
    void handledel(char);
    //查看好友列表
    void listfriend();
    //选定私聊页面
    void chatfriend();
    //输出聊天页面
    void chatmenu(char c, user& u2);
    //处理私聊功能
    void handlechat(char);
    //屏蔽/解除屏蔽好友
    void shieldfriend();
};

void friendfucs::addfriend(){
    Client* c = (Client*)clientp;
    Socket* sock = c->getSocket();
    system("clear");
    printf("\033[0;32m已选择添加好友功能(按ESC可返回上一级菜单)\033[0m\n");
    std::string red;
    char buf[520];
    char uid[256];
    printf("\033[0;32m请输入您要添加好友的uid:\n\033[0m>");
    while(1){
        chu(uid);
        if(enter(uid, 0) == -1) return ; 
        if(strlen(uid) <= 3){
            printf("\033[0;31m该uid无效，请重新输入。\033[0m\n>");
            continue;
        }
        if(uid == u.uid){
            printf("\033[0;31m不能添加自己为好友，请重新输入。\033[0m\n>");
            continue;
        }
        //判断此人是否存在
        sprintf(buf, "jrud:%s", uid);
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "norepeat"){
            printf("\033[0;31m该uid无效，请重新输入。\033[0m\n>");
            continue;
        }
        //判断是否已为好友
        chu(buf);
        sprintf(buf, "jrfd:%s:%s", uid, u.uid.c_str());
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "right"){
            printf("\033[0;31m您与%s用户已为好友，无需添加，请重新输入。\033[0m\n>", uid);
            continue;
        }
        
        chu(buf);
        red.clear();
        printf("\033[0;32m正在发送好友申请...\n\033[0m");
        fflush(stdout); // 手动刷新标准输出缓冲区
        //给服务器处理
        chu(buf);
        sprintf(buf, "adfr:%s:%s",u.uid.c_str(), uid);
        sock->sendMsg(buf);
        red = EchoMsgQueue.wait_and_pop();
        if(red == "right") {
            printf("\033[0;32m好友申请发送成功，请按任意键继续...\n\033[0m");
            char rub;
            rub = charget();
            return;
        } else{
            printf("\033[0;31m\n数据异常，请稍后再试。\033[0m\n");
            continue;
        }
    }
    return ;

}

void friendfucs::list(char c){
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

            printf("%s[%d]  %-12s %-14s %-12s\033[0m\n",
                    color, i - 5 * page + 1,
                    name.c_str(), ud.uid.c_str(), status.c_str());
        }
        i++;
    }
    printf("                     \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                \033[0;32m[%d/%d]\033[0m\n",page+1,maxpage);
    printf("\033[0;36m==========================================================\033[0m\n");
}

void friendfucs::listfriend(){
    system("clear");
    page = 0;
    list('0');
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    std::string msg;
    while(1){
        //判断用户信息是否变动
        if(UserMsgQueue.try_pop(msg)){
            page = 0;
            u = user::fromJson(msg);
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            list('p');
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        char input = tm_charget(200);
        if(input == -1) continue;
        switch(input){
        case '[':{
            system("clear");
            list('[');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
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

void friendfucs::handledel(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    //找到对应消息
    int i = 5*page + c - '0' - 1, j = 0;
    if(i >= u.friendlist.size()) return;
    std::string sd;
    for(std::string str : u.friendlist){
        if(j == i){
            sd = str;
            break;
        }
        j++;
    }
    sock->sendMsg("gtnm:"+sd);
    std::string nm = EchoMsgQueue.wait_and_pop(), rev;
    printf("\033[0;32m确定要删除好友\033[0m \033[0;31m%s\033[0m \033[0;32m？（Y/N）\033[0m\n", nm.c_str());
    fflush(stdout);
    char input;
    while(1){
        input = charget();
        if(input == 27) return;
        if(input != 'Y' && input != 'N' && input != 'y' && input != 'n') continue;
        if(input == 'Y' || input == 'y') break;
        else return;
        break;
    }
    //确定删除该好友,uid1删uid2
    sock->sendMsg("rmfd:"+u.uid+":"+sd);
    rev = EchoMsgQueue.wait_and_pop();
    if(rev != "right"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
    //删除本地列表中的uid2
    u.friendlist.erase(sd);
}


void friendfucs::delfriend(){
    system("clear");
    page = 0;
    list('0');
    printf("\033[0;32m请选择您要删除的好友:>\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    std::string msg;
    while(1){
        //判断用户信息是否变动
        if(UserMsgQueue.try_pop(msg)){
            page = 0;
            u = user::fromJson(msg);
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            list('p');
            printf("\033[0;32m请选择您要删除的好友:>\033[0m");
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
            handledel(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            list('[');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
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

void friendfucs::chatmenu(char c, user& ud2){
    reportfucs rpf(u, clientp);
    bool ret = rpf.Getrpt();
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    int cnt = 0;
    cnt = save.data.size();
    int maxpage = cnt / 7, i = 0;
    if(cnt % 7 != 0) maxpage++;
    if(maxpage == 0) maxpage = 1;
    if(c == '[' && ctpage == 0) ;
    else if(c == '[') ctpage --;
    if(c == ']' && ctpage+1 >= maxpage) ;
    else if(c == ']') ctpage ++;
    printf("\033[0;36m=============================聊天页面=============================\033[0m\n");
    reportfucs::newreport(u, clientp);
    if(ud2.stat == "online") printf("\033[0;32m                             %s (在线)\033[0m\n", ud2.name.c_str());
    else                     printf("\033[0;90m                             %s (离线)\033[0m\n", ud2.name.c_str());

    for(std::string str : save.data){
        if(i >= 7*ctpage && i < 7*(ctpage+1)){
            
        }
        i++;
    }
    printf("                                         \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                                         \033[0;32m[%d/%d]\033[0m\n",ctpage+1,maxpage);
    printf("\033[0;36m==================================================================\033[0m\n");
}

void friendfucs::handlechat(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    //找到对应uid
    int i = 5*page + c - '0' - 1, j = 0;
    if(i >= u.friendlist.size()) return;
    std::string uid2;
    for(std::string str : u.friendlist){
        if(j == i){
            uid2 = str;
            break;
        }
        j++;
    }
    sock->sendMsg("gtus:"+ uid2);
    std::string js2 = EchoMsgQueue.wait_and_pop();
    user ud2 = user::fromJson(js2);
    //开始聊天
    //拉取历史记录
    //读取两页消息（一页7句消息）
    if(uid2 > u.uid)
        sock->sendMsg("ctms:"+u.uid+":"+uid2);
    else sock->sendMsg("ctms:"+uid2+":"+u.uid);
    std::string rev = EchoMsgQueue.wait_and_pop(), msg;
    save = messages::fromJson(rev);
    //定义部分变量
    bool flag = false;
    system("clear");
    ctpage = 0;
    chatmenu('0', ud2);
    fflush(stdout); // 手动刷新标准输出缓冲区
    while(1){
        //判断用户信息是否变动
        if(UserMsgQueue.try_pop(msg)){
            page = 0;
            u = user::fromJson(msg);
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            chatmenu('p', ud2);
            printf("\033[0;32m请选择您要删除的好友:>\033[0m");
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
            //handledel(input);
            break;
        }
        case '[':{
            system("clear");
            list('[');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case 27:{
            return ;
        }
        default:continue;
        }
    }
}

void friendfucs::chatfriend(){
    system("clear");
    page = 0;
    list('0');
    printf("\033[0;32m请选择您要私聊的好友:>\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
    bool flag = false;
    std::string msg;
    while(1){
        //判断用户信息是否变动
        if(UserMsgQueue.try_pop(msg)){
            page = 0;
            u = user::fromJson(msg);
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            list('p');
            printf("\033[0;32m请选择您要私聊的好友:>\033[0m");
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
            handlechat(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            list('[');
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
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
