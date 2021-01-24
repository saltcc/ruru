#pragma once

#include <stdint.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

class RuruDtlsCtx
{
public:
    RuruDtlsCtx();
    ~RuruDtlsCtx();

    SSL_CTX *ctx;
    uint8_t certFingerPrint[96];
};

class RuruDtls
{
public:
    RuruDtls(RuruDtlsCtx *ctx);
    ~RuruDtls();

    void HandleDtlsPacket(const uint8_t *data, int32_t length);

    SSL* ssl;
    BIO* inBio;
    BIO* outBio;
    bool bHandShakeDone;
};