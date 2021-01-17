#include "RuruDtls.h"
#include "RuruCert.h"

RuruDtlsCtx::RuruDtlsCtx()
{
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    ctx = SSL_CTX_new(DTLSv1_method());
    if (ctx == NULL){
        ERR_print_errors_fp(stderr);
        return;
    }

    if (SSL_CTX_set_cipher_list(ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH") != 1){
        ERR_print_errors_fp(stderr);
        return;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

    RuruCert cert;

    if (SSL_CTX_use_PrivateKey(ctx, cert.key) != 1){
        ERR_print_errors_fp(stderr);
        return;
    }

    if (SSL_CTX_use_certificate(ctx, cert.x509) != 1){
        ERR_print_errors_fp(stderr);
        return;
    }

    if (SSL_CTX_check_private_key(ctx) != 1){
        ERR_print_errors_fp(stderr);
        return;
    }
    memcpy(certFingerPrint, cert.fingerPrint, sizeof(certFingerPrint));
}

RuruDtlsCtx::~RuruDtlsCtx()
{
    if (ctx){
        SSL_CTX_free(ctx);
        ctx = NULL;
    }
}

RuruDtls::RuruDtls()
{

}

RuruDtls::~RuruDtls()
{

}