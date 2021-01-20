#pragma once

#include "usrsctp.h"

class RuruClient;

class RuruSctp
{
public:
    RuruSctp(RuruClient *);
    ~RuruSctp();

    struct socket *sock;
    bool bHandShakeDone;
};
