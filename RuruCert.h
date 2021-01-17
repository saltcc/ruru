#pragma once

#include <openssl/x509.h>
#include <stdint.h>

class RuruCert
{
public:
    RuruCert();
    ~RuruCert();

    EVP_PKEY* key;
    X509* x509;
    char fingerPrint[96];
};