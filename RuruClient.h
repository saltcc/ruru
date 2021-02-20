#pragma once

#include "RuruDtls.h"
#include "RuruSctp.h"
#include "RuruNetwork.h"
#include "RuruEvent.h"
#include "RuruArena.h"
#include "RuruRingBuf.h"
#include <queue>
#include <memory>

struct HeartBeatData
{
    uint8_t data[1024];
    uint16_t length;
};

struct RuruConnectionData
{
    int32_t fd = -1;
    uint8_t data[4096] = {0};
    int32_t length = 0;
};

class RuruClient
{
public:
    RuruClient(RuruDtlsCtx *ctx, RuruAddress address, int32_t fd);
    ~RuruClient();

    RuruDtls dtlsTransport;
    RuruSctp sctpTransport;
    RuruRingBuf<HeartBeatData> heartBeatRing;

    RuruAddress address;
    std::queue<RuruEvent> *que;
    RuruArena *arena;
    std::queue<RuruCacheData *> cache;

    void HandleDtlsPacket(const uint8_t *data, int32_t length);
    void ClientSendData(const uint8_t *data, int32_t length);
    void ClientSendHeartBeatData();
private:
    void ClientSendPendingDtls();
    int32_t udpfd_;
};
