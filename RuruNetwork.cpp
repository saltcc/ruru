#include "RuruNetwork.h"
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

static int32_t CreateSocket(const uint8_t *port, struct addrinfo &hints)
{
    struct addrinfo* ainfo = nullptr;
    if (0 != getaddrinfo(nullptr, (const char *)port, &hints, &ainfo)){
        perror("getaddrinfo");
        return -1;
    }

    int32_t sfd = -1;

    struct addrinfo *addr = ainfo;
    for (; addr != nullptr; addr = addr->ai_next) {
        sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (-1 == sfd)
            continue;

        int32_t enable = 1;
        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
        if (0 == bind(sfd, addr->ai_addr, addr->ai_addrlen)){
            break;
        }

        if (sfd){
            close(sfd);
            sfd = -1;
        }
    }

    freeaddrinfo(ainfo);

    return sfd;
}

int32_t CreateTcpSocket(const uint8_t *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    return CreateSocket(port, hints);
}

int32_t CreateUdpSocket(const uint8_t *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    return CreateSocket(port, hints);
}

int32_t SetNonBlocking(int32_t sfd)
{
    int32_t flags = fcntl(sfd, F_GETFL, 0);
    if (-1 == flags){
        perror("fcntl F_GETFL");
        return -1;
    }

    flags |= O_NONBLOCK;

    if (-1 == fcntl(sfd, F_SETFL, flags)){
        perror("fcntl F_SETFL");
        return -1;
    }
    
    return 0;
}

uint32_t Ip2Host(const char *ip)
{
    return ntohl(inet_addr(ip));
}
const char *Host2Ip(uint32_t host)
{
    struct in_addr addr;
    addr.s_addr = htonl(host);
    return inet_ntoa(addr);
}