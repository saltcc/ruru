#include "RuruClient.h"

RuruClient::RuruClient(RuruDtlsCtx *ctx):dtlsTransport(ctx),sctpTransport(this)
{
}

RuruClient::~RuruClient()
{
}