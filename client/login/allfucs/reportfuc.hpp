#pragma once
#include<iostream>
#include <fcntl.h>
#include "../../client.h"
#include "../../../user.hpp"
#include <unistd.h>
#include <cstring>
#include "../MessageQueue.hpp"

class reportfucs{
private:
    user u;
    void* clientp;

public:
    report rpt;

    reportfucs(user arg1, void*p):u(arg1),clientp(p){
    };

    void friendreport();
    bool Getrpt();
};

bool reportfucs::Getrpt(){
    Client * cp = (Client*)clientp;
    Socket* sock = cp->getSocket();
    sock->sendMsg("gtrp:"+u.uid);
    //读取json字符串
    std::string js;
    js = EchoMsgQueue.wait_and_pop();
    if(js == "none") return false;
    rpt = report::fromJson(js);
    return true;
}

void reportfucs::friendreport(){

}