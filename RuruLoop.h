#pragma once

#include "RuruClient.h"
#include <vector>

class RuruLoop
{
public:
    RuruLoop(const uint8_t *host, const uint8_t *port);
    ~RuruLoop();

    int32_t Loop();
    void ClearAllClient();

private:
    int32_t udpfd_;
    void Destory();
    std::vector<RuruClient *> clientMgr_;
    static RuruDtlsCtx dtsCtx_;
    RuruClient *FindClientByAddress(RuruAddress address);
};