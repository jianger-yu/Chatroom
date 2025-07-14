#pragma once
#include "../MessageQueue.hpp"

class setupfucs{
private:
    user u;
    void* clientp;

public:

    setupfucs(user arg1, void*p):u(arg1),clientp(p){
    };

};