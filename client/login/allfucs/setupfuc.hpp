#pragma once
#include "../MessageQueue.hpp"

class setupfucs{
private:
    user& u;
    void* clientp;
    
public:

    setupfucs(user& arg1, void*p):u(arg1),clientp(p){
    };

    void ViewAccountInformation();


};

void setupfucs::ViewAccountInformation(){
    system("clear");
    printf("\033[0;32m以下为您的基本信息:\n\033[0m");
    printf("\033[0;32m用户名:\033[0m%s\n",u.name.c_str());
    printf("\033[0;32m电子邮箱:\033[0m%s\n",u.email.c_str());
    printf("\033[0;32muid:\033[0m%s\n",u.uid.c_str());
    printf("\033[0;32m请按任意键继续...\033[0m");
    charget();
}