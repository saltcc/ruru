#pragma once

#include "RuruClient.h"
#include "RuruEvent.h"
#include "RuruArena.h"
#include <vector>
#include <queue>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif
#include "picohttpparser.h"
#include "cJSON.h"
#ifdef __cplusplus
}
#endif

class RuruLoop
{
public:
    RuruLoop(const char *host, const char *port);
    ~RuruLoop();

    bool Loop(RuruEvent &evt);

private:
    int32_t udpfd_;
    int32_t tcpfd_;
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
    void HttpRequestHandle(RuruConnectionData* con);
    bool BodyParse(const char *data, int32_t length, RuruAddress &address);
};