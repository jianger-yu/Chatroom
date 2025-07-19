#pragma once
#include "../MessageQueue.hpp"
#include "reportfuc.hpp"
#include "../../../message.hpp"
#include <algorithm>

class friendfucs{
private:
    user &u;
    void* clientp;
    int page = 0;
    int ctpage = 0;
    int msgcnt = 0;
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
    //屏蔽/解除屏蔽好友，1为屏蔽，2为解除屏蔽
    void shieldfriend(int);
    //屏蔽功能
    void shield(char);
    //解除屏蔽功能
    void shield_exit(char);
    //屏蔽的好友的菜单
    void shield_menu(char);
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
    if(u.friendlist.size())
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
            if(u.friendlist.size())
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
            if(u.friendlist.size())
                printf("\033[0;32m请选择您要删除的好友:>\033[0m");
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
            if(u.friendlist.size())
                printf("\033[0;32m请选择您要删除的好友:>\033[0m");
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
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    reportfucs rpf(u, clientp);
    bool ret = rpf.Getrpt();
    if(ret){
        rpf.rpt.total_friend_msg -= rpf.rpt.chatfriend[ud2.uid];
        rpf.rpt.chatfriend[ud2.uid] = 0;
        sock->sendMsg("svrp:"+u.uid+":"+rpf.rpt.toJson());
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
    reportfucs::newreport(u, clientp);
    if(ud2.stat == "online") printf("\033[0;32m                             %s (在线)\033[0m\n", ud2.name.c_str());
    else                     printf("\033[0;90m                             %s (离线)\033[0m\n", ud2.name.c_str());
    if(ctpage+1 == maxctpage)    printf("\033[0;90m                       ---聊天记录已到顶---\033[0m\n");
    for(i = std::min(7*(ctpage+1) - 1, (int)save.data.size() - 1); i >= 7*ctpage; i--){
        if(!save.data.size()) break;
        message msg = message::fromJson(save.data[i]);
        sender = ud2.name;
        if(msg.sender_uid == u.uid) sender = u.name;
        if(ud2.stat == "online" || sender == u.name) printf("\033[0;32m%s\033[0m \033[0;33m[%s]\033[0m\n", sender.c_str(), msg.timestamp.c_str());
        else                     printf("\033[0;90m%s\033[0m \033[0;33m[%s]\033[0m\n", sender.c_str(), msg.timestamp.c_str());
        printf("\033[0;32m>\033[0m%s\n", msg.content.c_str());
    }
    printf("                                         \033[0;32m(tip:按[和]按键可控制翻页)\n\033[0m");
    printf("                                                         \033[0;32m[%d/%d]\033[0m\n",ctpage+1,maxctpage);
    printf("\033[0;36m==================================================================\033[0m\n");
}

void friendfucs::handlechat(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    printf("\033[0;32m数据请求中...\033[0m");
    fflush(stdout); // 手动刷新标准输出缓冲区
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
    sock->sendMsg("ctms:"+u.uid+":"+uid2);
    std::string rev = EchoMsgQueue.wait_and_pop(), msg;
    save = messages::fromJson(rev);
    //读取聊天记录总条数
    sock->sendMsg("rdpg:"+ u.uid + ":" + uid2);
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
            u = user::fromJson(msg);
            if(u.friendlist.count(uid2) == 0){//被删除了
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
            if(m.sender_uid == ud2.uid) save.data.insert(save.data.begin(), msg);
            msgcnt++;
            flag = true;
        }
        //判断是否有新通知
        if(ReptMsgQueue.try_pop(msg) || flag){
            flag = false;
            system("clear");
            chatmenu('p', ud2);
            printf("\033[0;32m请输入:>\033[0m");
            printf("%s", content.c_str());
            fflush(stdout); // 手动刷新标准输出缓冲区
        }
        char input = tm_charget(200);
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
            if(u.shieldlist.count(uid2)){
                system("clear");
                printf("\033[0;31m对方已被您屏蔽，发送失败！\033[0m\n");
                printf("\033[0;31m请解除屏蔽后再发送。\033[0m\n");
                printf("\033[0;31m请按任意键继续...\033[0m\n");
                charget();
                return;
            }
            content.push_back('\0');
            message sendm;
            sendm.sender_uid = u.uid;
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
                sprintf(tmp, "ndms:%s:%s:%ld", u.uid.c_str(), ud2.uid.c_str(), save.data.size());
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

void friendfucs::chatfriend(){
    system("clear");
    page = 0;
    list('0');
    if(u.friendlist.size())
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
            if(u.friendlist.size())
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
            if(u.friendlist.size())
                printf("\033[0;32m请选择您要私聊的好友:>\033[0m");
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            list(']');
            if(u.friendlist.size())
                printf("\033[0;32m请选择您要私聊的好友:>\033[0m");
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

void friendfucs::shield(char c){
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
    printf("\033[0;32m确定要屏蔽好友\033[0m \033[0;31m%s\033[0m \033[0;32m？（Y/N）\033[0m\n", nm.c_str());
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
    //确定屏蔽该好友,uid1屏蔽uid2
    sock->sendMsg("shfd:"+u.uid+":"+sd);
    rev = EchoMsgQueue.wait_and_pop();
    if(rev != "right"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
    //屏蔽本地列表中的uid2
    u.shieldlist.insert(sd);
    system("clear");
    printf("\033[0;32m屏蔽成功。\n\033[0m");
    printf("\033[0;32m请按任意键继续...\033[0m");
    charget();
}

void friendfucs::shield_menu(char c){
    reportfucs rpf(u, clientp);
    bool ret = rpf.Getrpt();
    Client* cp = (Client*) clientp;
    Socket* sock = cp->getSocket();
    if(!u.shieldlist.size()){
        printf("\033[0;32m当前没有屏蔽的好友。\n\033[0m");
        printf("\033[0;32m请按ESC返回...\033[0m");
        return;
    }
    int cnt = 0;
    cnt = u.shieldlist.size();
    int maxpage = cnt / 5, i = 0;
    if(cnt % 5 != 0) maxpage++;
    if(c == '[' && page == 0) ;
    else if(c == '[') page --;
    if(c == ']' && page+1 >= maxpage) ;
    else if(c == ']') page ++;
    printf("\033[0;36m==========================================================\033[0m\n");
    reportfucs::newreport(u, clientp);
    printf("\033[0;32m以下为您的黑名单\033[0m\n");
    printf("\033[0;34m%-6s %-15s %-13s %-12s\033[0m\n", "序号", "用户名", "UID", "在线状态");
    for(std::string str : u.shieldlist){
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

void friendfucs::shield_exit(char c){
    Client * cp = (Client*)clientp;
    Socket * sock = cp->getSocket();
    system("clear");
    //找到对应消息
    int i = 5*page + c - '0' - 1, j = 0;
    if(i >= u.shieldlist.size()) return;
    std::string sd;
    for(std::string str : u.shieldlist){
        if(j == i){
            sd = str;
            break;
        }
        j++;
    }
    sock->sendMsg("shex:"+sd);
    std::string nm = EchoMsgQueue.wait_and_pop(), rev;
    printf("\033[0;32m确定要解除屏蔽好友\033[0m \033[0;31m%s\033[0m \033[0;32m？（Y/N）\033[0m\n", nm.c_str());
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
    //确定屏蔽该好友,uid1屏蔽uid2
    sock->sendMsg("shex:"+u.uid+":"+sd);
    rev = EchoMsgQueue.wait_and_pop();
    if(rev != "right"){
        printf("\033[0;31m数据异常，请稍后再试。\033[0m\n");
        printf("\033[0;31m请按任意键继续...\033[0m");
        input = charget();
        return ;
    }
    //去除本地黑名单中的uid2
    u.shieldlist.erase(sd);
    system("clear");
    printf("\033[0;32m解除屏蔽成功。\n\033[0m");
    printf("\033[0;32m请按任意键继续...\033[0m");
    charget();
}

void friendfucs::shieldfriend(int cs){
    system("clear");
    page = 0;
    if(cs == 1){
        list('0');
        if(u.friendlist.size())
            printf("\033[0;32m请选择您要屏蔽的好友:>\033[0m");
    } else if(cs == 2){
        shield_menu('0');
        if(u.shieldlist.size())
            printf("\033[0;32m请选择您要解除屏蔽的好友:>\033[0m");
    }
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
            if(cs == 1){
                list('p');
                if(u.friendlist.size())
                    printf("\033[0;32m请选择您要屏蔽的好友:>\033[0m");
            } else if(cs == 2){
                shield_menu('0');
                if(u.shieldlist.size())
                    printf("\033[0;32m请选择您要解除屏蔽的好友:>\033[0m");
            }
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
            if(cs == 1)
                shield(input);
            else if(cs == 2)
                shield_exit(input);
            flag = true;
            break;
        }
        case '[':{
            system("clear");
            if(cs == 1){
                list('[');
                if(u.friendlist.size())
                    printf("\033[0;32m请选择您要屏蔽的好友:>\033[0m");
            } else if(cs == 2){
                shield_menu('0');
                if(u.shieldlist.size())
                    printf("\033[0;32m请选择您要解除屏蔽的好友:>\033[0m");
            }
            fflush(stdout); // 手动刷新标准输出缓冲区
            break;
        }
        case ']':{
            system("clear");
            if(cs == 1){
                list(']');
                if(u.friendlist.size())
                    printf("\033[0;32m请选择您要屏蔽的好友:>\033[0m");
            } else if(cs == 2){
                shield_menu('0');
                if(u.shieldlist.size())
                    printf("\033[0;32m请选择您要解除屏蔽的好友:>\033[0m");
            }
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
