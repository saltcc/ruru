#include "RuruLoop.h"
#include "RuruNetwork.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/epoll.h>

static const int32_t MAX_EVENT = 1000;
RuruDtlsCtx RuruLoop::dtsCtx_;

RuruLoop::RuruLoop(const uint8_t *host, const uint8_t *port)
{
    udpfd_ = CreateUdpSocket(host, port);
    if (udpfd_ < 0){
        perror("RuruLoop CreateUdpSocket");
        return;
    }

    int32_t ret = SetNonBlocking(udpfd_);
    if (ret < 0){
        perror("RuruLoop SetNonBlocking");
        Destory();
        return;
    }

    epfd_ = epoll_create1(0);
    if (epfd_ < 0){
        perror("RuruLoop epoll_create1");
        Destory();
        return;
    }

    struct epoll_event event;
    event.data.fd = udpfd_;
    if (-1 == epoll_ctl(epfd_, EPOLL_CTL_ADD, udpfd_, &event)){
        perror("RuruLoop epoll_ctl");
        Destory();
    }
    maxEvents_ = MAX_EVENT;
    events_ = new epoll_event[maxEvents_];

    RuruSctp::UsrsctpStartInit();

    //test TODO
    RuruAddress address;
    address.host = ntohl((uint32_t)inet_addr("192.168.28.128"));
    address.port = 8000;
    RuruClient *client = new RuruClient(&RuruLoop::dtsCtx_, address, udpfd_);
    AttachClientInfo(client);
    clientMgr_.push_back(client);
}

RuruLoop::~RuruLoop()
{
    Destory();
}

void RuruLoop::Destory()
{
    if (udpfd_ > 0){
        close(udpfd_);
        udpfd_ = -1;
    }

    if (events_){
        delete []events_;
        events_ = nullptr;
    }

    ClearAllClient();
}

bool RuruLoop::UpdateEvent(RuruEvent &evt)
{
    bool bHaveData = false;

    if (!que_.empty()){
        evt = que_.front();
        que_.pop();
        bHaveData = true;
        return bHaveData;
    }

    arena_.ArenaReset();

    return bHaveData;
}

bool RuruLoop::EpollWait()
{
    bool bHaveData = false;

    int32_t num = epoll_wait(epfd_, events_, MAX_EVENT, 0);
    for (int32_t i = 0; i < num; i++) {
        struct epoll_event* e = &events_[i];
        if (e->data.fd == udpfd_){
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

                bHaveData = true;
            }
        }
    }

    return bHaveData;
}

bool RuruLoop::Loop(RuruEvent &evt)
{
 
    bool bHaveData = false;

    if (UpdateEvent(evt)){
        bHaveData = true;
        return bHaveData;
    }

    bHaveData |= EpollWait();

    return bHaveData;
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
        if (*tmp != nullptr){
            delete *tmp;
        }
    }
    clientMgr_.clear();
}


void RuruLoop::AttachClientInfo(RuruClient *client)
{
    client->que = &this->que_;
    client->arena = &this->arena_;
}
