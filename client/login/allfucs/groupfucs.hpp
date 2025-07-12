#pragma once

class groupfucs{
private:
    user u;
    void* clientp;

public:

    groupfucs(user arg1, void*p):u(arg1),clientp(p){
    };

};

class recordfucs{
private:
    user u;
    void* clientp;

public:

    recordfucs(user arg1, void*p):u(arg1),clientp(p){
    };

};
