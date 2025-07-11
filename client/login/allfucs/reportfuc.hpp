#pragma once
#include "allfucs.h"

class reportfucs{
private:
    user u;
    void* clientp;

public:

    reportfucs(user arg1, void*p):u(arg1),clientp(p){
    };

};