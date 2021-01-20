#pragma once

#include "RuruDtls.h"
#include "RuruSctp.h"

class RuruClient
{
public:
    RuruClient(RuruDtlsCtx *ctx);
    ~RuruClient();

    RuruDtls dtlsTransport;
    RuruSctp sctpTransport;
};
