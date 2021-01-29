#include "RuruLoop.h"
#include "RuruNetwork.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

RuruDtlsCtx RuruLoop::dtsCtx_;

RuruLoop::RuruLoop(const uint8_t *host, const uint8_t *port)
{
    udpfd_ = CreateUdpSocket(port);
    if (udpfd_ < 0){
        perror("RuruLoop CreateUdpSocket");
        return;
    }

    std::cout<<"CreateUdpSocket ok"<<std::endl;

    int32_t ret = SetNonBlocking(udpfd_);
    if (ret < 0){
        perror("RuruLoop SetNonBlocking");
        Destory();
        return;
    }
    std::cout<< "SetNonBlocking ok"<<std::endl;
    RuruSctp::UsrsctpStartInit();
}

RuruLoop::~RuruLoop()
{
    ClearAllClient();
}

void RuruLoop::Destory()
{
    if (udpfd_ > 0){
        close(udpfd_);
        udpfd_ = -1;
    }
}

int32_t RuruLoop::Loop()
{
    RuruAddress address;
    address.host = ntohl((uint32_t)inet_addr("192.168.28.128"));
    address.port = 8000;
    RuruClient *client = new RuruClient(&RuruLoop::dtsCtx_, address, udpfd_);
    clientMgr_.push_back(client);

    while(1)
    {
        struct sockaddr_in remote;
        socklen_t remoteLen = sizeof(remote);

        ssize_t bytes = 0;
        uint8_t buffer[4096];
        while((bytes = recvfrom(udpfd_, buffer, sizeof(buffer), 0, (struct sockaddr*)&remote, &remoteLen)) > 0){
            RuruAddress address;
            address.host = ntohl(remote.sin_addr.s_addr);
            address.port = ntohs(remote.sin_port);

            RuruClient *client = FindClientByAddress(address);
            if (client){
                client->HandleDtlsPacket(buffer, bytes);
            }
        }
    }
}

RuruClient *RuruLoop::FindClientByAddress(RuruAddress address)
{
    for (std::vector<RuruClient *>::iterator it = clientMgr_.begin(); it != clientMgr_.end(); ++it){
        RuruClient *client = *it;
        if (client->address.host == address.host && client->address.port == address.port){
            return client;
        }
    }
    return nullptr;
}

void RuruLoop::ClearAllClient()
{
    for (std::vector<RuruClient *>::iterator it = clientMgr_.begin(); it != clientMgr_.end(); ++it){
        std::vector<RuruClient *>::iterator tmp = it;
        if (*tmp != NULL){
            delete *tmp;
        }
    }
    clientMgr_.clear();
}

