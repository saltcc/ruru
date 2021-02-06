#pragma once

#include "usrsctp.h"

struct RuruSctpMessage {
    void *data = nullptr;
    size_t len = 0;
    uint16_t sid = 0;
    uint32_t ppid = 0;
};

class RuruClient;

class RuruSctp
{
public:
    RuruSctp(RuruClient *);
    ~RuruSctp();

    struct socket *sock;
    bool bHandShakeDone;

    static void UsrsctpStartInit();
    bool SendUsrSctpData(RuruSctpMessage *sctpMsg);
    void RecvUsrSctpData(const uint8_t *data, int32_t length);
    RuruClient *client;
    bool OpenSctp();

private:
    static int32_t OnSctpOutboundPacket(void *addr, void *data, size_t len, uint8_t tos, uint8_t set_df);
    static int32_t OnSctpInboundPacket(struct socket* sock,union sctp_sockstore addr,void* data,size_t length,struct sctp_rcvinfo rcv,int flags,void* ulp_info);
    static void UsrsctpUnInit();
    bool ConfigureSctpSocket();
    void CloseSctpSocket();
    static bool usrsctpInit_;
    struct socket *sock_;
};
