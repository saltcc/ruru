#include <iostream>
#include <stdint.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <usrsctp.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include "RuruNetwork.h"

SSL_CTX *ctx;
SSL* ssl;
BIO* inBio;
BIO* outBio;

int32_t fd = -1;
struct sockaddr_in ser_addr;

#define BUFF_LEN 2048

void SendPendingDtls();
bool doHandeShake();
bool opensslLabInit();
bool dtlsTransInit();
bool socketCreateSet();

int main()
{
    opensslLabInit();
    dtlsTransInit();
    socketCreateSet();

    bool handshake = doHandeShake();
    if (!handshake){
         perror("handshake ok");
         return -1;
    }
    std::cout<<"hand ok"<<std::endl;

    return 0;
}

void SendPendingDtls()
{
    uint8_t buffer[4096];
    while (BIO_ctrl_pending(outBio) > 0) {
        int32_t bytes = BIO_read(outBio, buffer, sizeof(buffer));
        if (bytes > 0) {
            sendto(fd, buffer, bytes, 0, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
        }
    }
}

bool doHandeShake()
{
    uint8_t buf[BUFF_LEN] = {0};
    socklen_t len;
    while(1)
    {
        int32_t length = recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&ser_addr, &len);
        if (length > 0)
        {
            BIO_write(inBio, buf, length);
        }

        if (!SSL_is_init_finished(ssl))
        {
            int r = SSL_do_handshake(ssl);
            if (r <= 0) {
                r = SSL_get_error(ssl, r);
                if (SSL_ERROR_WANT_READ == r) {
                    SendPendingDtls();
                }
                else if((SSL_ERROR_NONE != r)){
                    return false;
                }
            }
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool opensslLabInit()
{
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    return true;
}

bool dtlsTransInit()
{
    ctx = SSL_CTX_new(DTLSv1_method());
    SSL_CTX_set_read_ahead(ctx, 1);
    SSL_CTX_set_ecdh_auto(ctx, 1);
    SSL_CTX_set_cipher_list(ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

    ssl = SSL_new(ctx);
    inBio = BIO_new(BIO_s_mem());
    BIO_set_mem_eof_return(inBio, -1);
    outBio = BIO_new(BIO_s_mem());
    BIO_set_mem_eof_return(outBio, -1);
    SSL_set_bio(ssl, inBio, outBio);
    SSL_set_connect_state(ssl);

    SSL_set_tmp_ecdh(ssl, EC_KEY_new_by_curve_name(NID_X9_62_prime256v1));
    SSL_set_options(ssl, SSL_OP_SINGLE_ECDH_USE);

    return true;
}

bool socketCreateSet()
{
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("192.168.28.128");
    ser_addr.sin_port = htons(9999);

    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket");
        return false;
    }
    
    struct sockaddr_in cli_addr;
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(8000);
    cli_addr.sin_addr.s_addr = inet_addr("192.168.28.128");
    int32_t ret = bind(fd, (struct sockaddr* )&cli_addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        perror("bind");
        return false;
    }

    SetNonBlocking(fd);

    return true;
}