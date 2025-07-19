#pragma once
#include "../MessageQueue.hpp"

class filefucs{
private:
    user& u;
    void* clientp;

public:

    filefucs(user& arg1, void*p):u(arg1),clientp(p){
    };

};

