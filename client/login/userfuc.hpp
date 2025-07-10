#pragma once
#include<iostream>
#include <fcntl.h>
#include "../client.h"
#include "../../user.hpp"
#include "../register/SendEmail.hpp"
#include <unistd.h>
#include <cstring>

class userfuc{
private:
    user u;


public:
    userfuc(){
    };

    int mainfuc();

};

int userfuc::mainfuc(){
    printf("登录成功！！!");
    char a;
    a = charget();
    return 0;
}