#pragma once

#include "RuruDtls.h"
#include "RuruSctp.h"
#include "RuruNetwork.h"
#include "RuruEvent.h"
#include "RuruArena.h"
#include <queue>
#include <memory>
class RuruClient
{
public:
    RuruClient(RuruDtlsCtx *ctx, RuruAddress address, int32_t fd);
    ~RuruClient();

    RuruDtls dtlsTransport;
    RuruSctp sctpTransport;

    RuruAddress address;
    std::queue<RuruEvent> *que;
    RuruArena *arena;
    std::queue<RuruCacheData *> cache;

    void HandleDtlsPacket(const uint8_t *data, int32_t length);
    void ClientSendData(const uint8_t *data, int32_t length);
private:
    void ClientSendPendingDtls();
    int32_t udpfd_;
};
