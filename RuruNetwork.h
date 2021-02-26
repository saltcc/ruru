#pragma once

#include <stdint.h>
#include <iostream>

struct RuruAddress
{
    uint32_t host;
    uint16_t port;

    RuruAddress(){
        host = 0;
        port = 0;
    }
};
int32_t CreateUdpSocket(const char *host, const char *port);
int32_t CreateTcpSocket(const char *host, const char *port);
int32_t SetNonBlocking(int32_t sfd);
uint32_t Ip2Host(const char *ip);
const char *Host2Ip(uint32_t host);
ssize_t SocketWrite(int fd, const uint8_t* buf, size_t len);