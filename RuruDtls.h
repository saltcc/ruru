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

    SSL* ssl;
    BIO* inBio;
    BIO* outBio;
    bool bHandShakeDone;
};