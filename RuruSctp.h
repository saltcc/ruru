#pragma once

#include "usrsctp.h"

class RuruClient;

class RuruSctp
{
public:
    RuruSctp(RuruClient *);
    ~RuruSctp();

    struct socket *sock;
    bool bHandShakeDone;

    static void UsrsctpStartInit();
    int32_t SendUsrSctpData(const uint8_t *data, int32_t length);

private:
    static int32_t OnSctpOutboundPacket(void *addr, void *data, size_t len, uint8_t tos, uint8_t set_df);
    static int32_t OnSctpInboundPacket(struct socket* sock,union sctp_sockstore addr,void* data,size_t length,struct sctp_rcvinfo rcv,int flags,void* ulp_info);
    static void UsrsctpUnInit();
    bool ConfigureSctpSocket();
    void CloseSctpSocket();
    static bool usrsctpInit_;
    struct socket *sock_;
    RuruClient *client_;
    
};
