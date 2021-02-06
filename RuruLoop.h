#pragma once

#include "RuruClient.h"
#include "RuruEvent.h"
#include "RuruArena.h"
#include <vector>
#include <queue>
#include <memory>

class RuruLoop
{
public:
    RuruLoop(const uint8_t *host, const uint8_t *port);
    ~RuruLoop();

    bool Loop(RuruEvent &evt);

private:
    int32_t udpfd_;
    std::vector<std::unique_ptr<RuruClient>> clientMgr_;
    static RuruDtlsCtx dtsCtx_;
    std::queue<RuruEvent> que_;
    RuruArena arena_;
    int32_t epfd_;
    struct epoll_event* events_;
    int32_t maxEvents_;

    void AttachClientInfo(RuruClient *client);
    bool UpdateEvent(RuruEvent &evt);
    bool EpollWait();
    RuruClient *FindClientByAddress(RuruAddress address);
    void Destory();
    bool ScanClientCacheData();
};