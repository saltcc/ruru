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

RuruDtls::RuruDtls(RuruDtlsCtx *dtlsCtx)
{
    ssl = SSL_new(dtlsCtx->ctx);
    inBio = BIO_new(BIO_s_mem());
    BIO_set_mem_eof_return(inBio, -1);
    outBio = BIO_new(BIO_s_mem());
    BIO_set_mem_eof_return(outBio, -1);
    SSL_set_bio(ssl, inBio, outBio);
    SSL_set_options(ssl, SSL_OP_SINGLE_ECDH_USE);
    SSL_set_options(ssl, SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
    SSL_set_tmp_ecdh(ssl, EC_KEY_new_by_curve_name(NID_X9_62_prime256v1));
    SSL_set_accept_state(ssl);
    SSL_set_mtu(ssl, 1400);
    bHandShakeDone = false;
}

RuruDtls::~RuruDtls()
{
    SSL_free(ssl);
    ssl = NULL;
    inBio = NULL;
    outBio = NULL;
}