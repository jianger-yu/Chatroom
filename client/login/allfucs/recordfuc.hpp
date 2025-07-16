#pragma once
#include "../MessageQueue.hpp"

class recordfucs{
private:
    user& u;
    void* clientp;

public:

    recordfucs(user& arg1, void*p):u(arg1),clientp(p){
    };

};