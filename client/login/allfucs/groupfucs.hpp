#pragma once
#include "../MessageQueue.hpp"

class groupfucs{
private:
    user& u;
    void* clientp;

public:

    groupfucs(user& arg1, void*p):u(arg1),clientp(p){
    };

};


