#include "RuruClient.h"

RuruClient::RuruClient(RuruDtlsCtx *ctx, RuruAddress address, int32_t fd):dtlsTransport(ctx),sctpTransport(this)
{
    udpfd_ = fd;
    address = address;
}

RuruClient::~RuruClient()
{
}

void RuruClient::ClientSendPendingDtls()
{
    uint8_t buffer[4096];
    while (BIO_ctrl_pending(dtlsTransport.outBio) > 0) {
        int32_t bytes = BIO_read(dtlsTransport.outBio, buffer, sizeof(buffer));
        if (bytes > 0) {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = htonl(address.host);
            addr.sin_port = htons(address.port);
            sendto(udpfd_, buffer, bytes, 0, (struct sockaddr*)&addr, sizeof(addr));
        }
    }
}

void RuruClient::HandleDtlsPacket(const uint8_t *data, int32_t length)
{
    BIO_write(dtlsTransport.inBio, data, length);

    if (!SSL_is_init_finished(dtlsTransport.ssl)){
        int r = SSL_do_handshake(dtlsTransport.ssl);
        if (r <= 0) {
            r = SSL_get_error(dtlsTransport.ssl, r);
            if (SSL_ERROR_WANT_READ == r) {
                ClientSendPendingDtls();
            }
            else if((SSL_ERROR_NONE != r)){
                printf("%s\n",ERR_error_string(r, nullptr));
            }
        }
    }
    else{
        dtlsTransport.bHandShakeDone = true;
        ClientSendPendingDtls();
        while (BIO_ctrl_pending(dtlsTransport.inBio) > 0) {
            uint8_t buffer[4096];
            int bytes = SSL_read(dtlsTransport.ssl, buffer, sizeof(buffer));
            if (bytes > 0) {
                sctpTransport.RecvUsrSctpData(data, length);
            }
        }
    }
}

void RuruClient::ClientSendData(const uint8_t *data, int32_t length)
{
    if (dtlsTransport.bHandShakeDone){
        SSL_write(dtlsTransport.ssl, data, length);
        ClientSendPendingDtls();
    }
}