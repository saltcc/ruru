#include "RuruSctp.h"
#include "RuruClient.h"

enum PayloadProtocolIdentifier {
  PPID_NONE = 0,  // No protocol is specified.
  // Matches the PPIDs in mozilla source and
  // https://datatracker.ietf.org/doc/draft-ietf-rtcweb-data-protocol Sec. 9
  // They're not yet assigned by IANA.
  PPID_CONTROL = 50,
  PPID_BINARY_PARTIAL = 52,
  PPID_BINARY_LAST = 53,
  PPID_TEXT_PARTIAL = 54,
  PPID_TEXT_LAST = 51
};

RuruSctp::RuruSctp(RuruClient *client)
{
    if (!client){
        return;
    }
    client_ = client;

    if (!usrsctpInit_){
        UsrsctpStartInit();
    }

    sock_ = usrsctp_socket(AF_CONN, SOCK_STREAM, IPPROTO_SCTP, &RuruSctp::OnSctpInboundPacket, NULL, 0, this);

    if (sock_ == NULL){
        perror("usrsctp_socket");
        return;
    }

    if (!ConfigureSctpSocket()){
        perror("ConfigureSctpSocket");
        CloseSctpSocket();
        return;
    }
    if (usrsctp_listen(sock_, 1) < 0){
        perror("usrsctp_listen");
        CloseSctpSocket();
        return;
    }

    usrsctp_register_address(this);
    bHandShakeDone = false;
}

RuruSctp::~RuruSctp()
{
}

int32_t RuruSctp::OnSctpOutboundPacket(void *addr, void *data, size_t len, uint8_t tos, uint8_t set_df)
{
    return 0;
}

int32_t RuruSctp::OnSctpInboundPacket(struct socket* sock,
                                union sctp_sockstore addr,
                                void* data,
                                size_t length,
                                struct sctp_rcvinfo rcv,
                                int flags,
                                void* ulp_info)
{
    RuruSctp* transport = static_cast<RuruSctp*>(ulp_info);
    (void)transport;
    return 0;
}

void RuruSctp::UsrsctpStartInit()
{
    if (!usrsctpInit_){
        usrsctp_init(0, &RuruSctp::OnSctpOutboundPacket, NULL);
        usrsctp_sysctl_set_sctp_ecn_enable(0);
        usrsctpInit_ = true;
    }
    
}

bool RuruSctp::ConfigureSctpSocket()
{
    if (usrsctp_set_non_blocking(sock_, 1) < 0) {
        perror("ConfigureSctpSocket usrsctp_set_non_blocking fail");
        return false;
    }

    struct linger lopt;
    lopt.l_onoff = 1;
    lopt.l_linger = 0;
    usrsctp_setsockopt(sock_, SOL_SOCKET, SO_LINGER, &lopt, sizeof(lopt));

    struct sctp_paddrparams paddparam;
    memset(&paddparam, 0, sizeof(paddparam));
    paddparam.spp_flags = SPP_PMTUD_DISABLE;
    paddparam.spp_pathmtu = 1200;
    usrsctp_setsockopt(sock_, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &paddparam, sizeof(paddparam));

    struct sctp_assoc_value stream_rst;
    stream_rst.assoc_id = SCTP_ALL_ASSOC;
    stream_rst.assoc_value = 1;
    usrsctp_setsockopt(sock_, IPPROTO_SCTP, SCTP_ENABLE_STREAM_RESET,&stream_rst, sizeof(stream_rst));

    uint32_t nodelay = 1;
    usrsctp_setsockopt(sock_, IPPROTO_SCTP, SCTP_NODELAY, &nodelay, sizeof(nodelay));

    uint32_t eor = 1;
    usrsctp_setsockopt(sock_, IPPROTO_SCTP, SCTP_EXPLICIT_EOR, &eor, sizeof(eor));

    struct sockaddr_conn sconn;
    memset(&sconn, 0, sizeof(sconn));
    sconn.sconn_family = AF_CONN;
    sconn.sconn_port = 5001;
    sconn.sconn_addr = (void *)this;
    if (usrsctp_bind(sock_, (struct sockaddr *)&sconn, sizeof(sconn)) < 0){
        perror("usrsctp_bind");
        return false;
    }

    return true;
}

void RuruSctp::CloseSctpSocket()
{
    if(sock_){
        usrsctp_close(sock_);
        sock_ = nullptr;
        usrsctp_deregister_address(client_);
    }
}

void RuruSctp::UsrsctpUnInit()
{
    for (int32_t i = 0; i < 300; ++i){
        if (usrsctp_finish() == 0){
            return;
        }
    }
}

int32_t RuruSctp::SendUsrSctpData(const uint8_t *data, int32_t length)
{
    return length;
}