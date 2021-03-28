#pragma once

#include "usrsctp.h"
#include <memory>

struct SctpHeader{
    uint16_t srcport;
    uint16_t dstport;
    uint32_t vertag;
    uint32_t checksum;
    uint8_t chunktype;
    uint8_t chunkflag;
    uint16_t chunklen;
};

struct SctpMsgParam{
    uint16_t sid = 0;
    uint32_t ppid = 0;
};

struct RuruSctpMessage {
    void *data = nullptr;
    size_t len = 0;
    SctpMsgParam param;
};

class RuruClient;

struct RuruCacheData
{
    RuruCacheData(){
        data = nullptr;
        length = 0;
    }
    ~RuruCacheData(){
        if (data){
            delete []data;
            data = nullptr;
        }
    }
    SctpMsgParam param;
    uint8_t *data;
    int32_t length;
};

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
    bool SendSctpCacheData();
    RuruClient *client;
    bool OpenSctp();

private:
    static int32_t OnSctpOutboundPacket(void *addr, void *data, size_t len, uint8_t tos, uint8_t set_df);
    static int32_t OnSctpInboundPacket(struct socket* sock,union sctp_sockstore addr,void* data,size_t length,struct sctp_rcvinfo rcv,int flags,void* ulp_info);
    static void UsrsctpUnInit();
    bool ConfigureSctpSocket();
    void CloseSctpSocket();
    void PushMsg2ClientCache(RuruSctpMessage *msg);
    static bool usrsctpInit_;
    struct socket *sock_;
};
