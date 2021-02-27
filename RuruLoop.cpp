#include "RuruLoop.h"
#include "RuruNetwork.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/epoll.h>

static const int32_t MAX_EVENT = 1000;
RuruDtlsCtx RuruLoop::dtsCtx_;

RuruLoop::RuruLoop(const char *host, const char *port)
{
    udpfd_ = CreateUdpSocket(host, port);
    if (udpfd_ < 0){
        perror("RuruLoop CreateUdpSocket");
        return;
    }

    int32_t ret = SetNonBlocking(udpfd_);
    if (ret < 0){
        perror("RuruLoop udp SetNonBlocking");
        Destory();
        return;
    }

    tcpfd_ = CreateTcpSocket(host, port);
    if (tcpfd_ < 0){
        perror("RuruLoop CreateTcpSocket");
        Destory();
        return;
    }

    ret = SetNonBlocking(tcpfd_);
    if (ret < 0){
        perror("RuruLoop tcp SetNonBlocking");
        Destory();
        return;        
    }

    ret = listen(tcpfd_, SOMAXCONN);
    if (ret < 0){
        perror("RuruLoop tcp listen");
        Destory();
        return;
    }

    epfd_ = epoll_create1(0);
    if (epfd_ < 0){
        perror("RuruLoop epoll_create1");
        Destory();
        return;
    }

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    RuruConnectionData *ucon = new RuruConnectionData;
    ucon->fd = udpfd_;
    event.data.ptr = ucon;
    event.events = EPOLLIN | EPOLLET;
    if (-1 == epoll_ctl(epfd_, EPOLL_CTL_ADD, udpfd_, &event)){
        perror("RuruLoop udp epoll_ctl");
        Destory();
        return;
    }

    RuruConnectionData *con = new RuruConnectionData;
    con->fd = tcpfd_;
    memset(&event, 0, sizeof(event));
    event.data.ptr = con;
    event.events = EPOLLIN | EPOLLET;
    if (-1 == epoll_ctl(epfd_, EPOLL_CTL_ADD, tcpfd_, &event)){
        perror("RuruLoop tcp epoll_ctl");
        Destory();
        return;
    }

    maxEvents_ = MAX_EVENT;
    events_ = new epoll_event[maxEvents_];

    RuruSctp::UsrsctpStartInit();
}

RuruLoop::~RuruLoop()
{
    Destory();
}

void RuruLoop::Destory()
{
    if (udpfd_ > 0){
        close(udpfd_);
        udpfd_ = -1;
    }

    if (tcpfd_ > 0){
        close(tcpfd_);
        tcpfd_ = -1;
    }

    if (events_){
        delete []events_;
        events_ = nullptr;
    }
}

bool RuruLoop::UpdateEvent(RuruEvent &evt)
{
    bool bHaveData = false;

    if (!que_.empty()){
        evt = que_.front();
        que_.pop();
        bHaveData = true;
        return bHaveData;
    }

    arena_.ArenaReset();

    return bHaveData;
}

bool RuruLoop::EpollWait()
{
    bool bHaveData = false;

    int32_t num = epoll_wait(epfd_, events_, MAX_EVENT, 0);
    for (int32_t i = 0; i < num; i++) {
        struct epoll_event* e = &events_[i];
        RuruConnectionData *con = static_cast<RuruConnectionData *>(e->data.ptr);

        if ((e->events & EPOLLERR) || (e->events & EPOLLHUP) || (!(e->events & EPOLLIN))) {
            close(con->fd);
            delete con;
            con = NULL;
            continue;
        }

        if (con->fd == udpfd_){
            struct sockaddr_in remote;
            socklen_t remoteLen = sizeof(remote);
            ssize_t bytes = 0;
            uint8_t buffer[4096];
            while((bytes = recvfrom(udpfd_, buffer, sizeof(buffer), 0, (struct sockaddr*)&remote, &remoteLen)) > 0){
                RuruAddress address;
                address.host = ntohl(remote.sin_addr.s_addr);
                address.port = ntohs(remote.sin_port);
                RuruClient *client = FindClientByAddress(address);
                if (client){
                    client->HandleDtlsPacket(buffer, bytes);
                }

                bHaveData = true;
            }
        }
        else if (con->fd == tcpfd_){
            for (;;) {
                struct sockaddr_in saddr;
                socklen_t addlen = sizeof(saddr);

                int cfd = accept(tcpfd_, (struct sockaddr*)&saddr, &addlen);
                if (cfd == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        perror("accept");
                    }
                    break;
                }

                if (SetNonBlocking(cfd) == -1) {
                    close(cfd);
                    perror("client cfd SetNonBlocking");
                    continue;
                }

                RuruConnectionData* con = new RuruConnectionData;

                if (con) {
                    con->fd = cfd;
                    struct epoll_event event;
                    event.events = EPOLLIN | EPOLLET;
                    event.data.ptr = con;
                    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, cfd, &event) == -1) {
                        close(cfd);
                        perror("client cfd epoll_ctl");
                    }
                } else {
                    close(cfd);
                }
            }
            bHaveData = true;
        }
        else{
            HttpRequestHandle(con);
            bHaveData = true;
        }    
    }

    return bHaveData;
}

bool RuruLoop::Loop(RuruEvent &evt)
{
 
    bool bHaveData = false;

    if (UpdateEvent(evt)){
        bHaveData = true;
        return bHaveData;
    }
    bHaveData |= ScanClientCacheData();
    bHaveData |= EpollWait();

    return bHaveData;
}

RuruClient *RuruLoop::FindClientByAddress(RuruAddress address)
{
    for (std::vector<std::unique_ptr<RuruClient>>::iterator it = clientMgr_.begin(); it != clientMgr_.end(); ++it){
        RuruClient *client = (*it).get();
        if (client->address.host == address.host && client->address.port == address.port){
            return client;
        }
    }
    return nullptr;
}

void RuruLoop::AttachClientInfo(RuruClient *client)
{
    client->que = &this->que_;
    client->arena = &this->arena_;
}

bool RuruLoop::ScanClientCacheData()
{
    int32_t bHaveData = false;
    static int32_t count = 0;
    static int32_t roll = 100;
    if (count++ % roll != 0){
        return bHaveData;
    }

    for (std::vector<std::unique_ptr<RuruClient>>::iterator it = clientMgr_.begin(); it != clientMgr_.end(); ++it){
        RuruClient *client = (*it).get();
        if (client && !client->cache.empty()){
            RuruEvent evt;
            evt.client = client;
            evt.type = EVT_SendCacheData;
            client->que->push(evt);
            bHaveData = true;
        }
    }

    return bHaveData;
}

void RuruLoop::HttpRequestHandle(RuruConnectionData* con)
{
    static const size_t HTTP_MAX_SIZE = 4096;
    static const char *HTTP_BAD_REQUEST = "HTTP/1.1 400 Bad request\r\n\r\n";

    for (;;){
        ssize_t length = read(con->fd, con->data + con->length, HTTP_MAX_SIZE - con->length);

        if ((length < 0 && errno != EAGAIN) || length == 0){
            close(con->fd);
            delete con;
            return;
        }

        size_t lastLen = con->length;
        con->length += length;

        const char *method = NULL;
        size_t methodLen = 0;
        const char *path = NULL;
        size_t pathLen = 0;
        int32_t minorVersion = 0;
        size_t numHeaders = 16;
        struct phr_header headers[16];

        int32_t status = phr_parse_request((const char *)con->data, con->length, 
                                &method, &methodLen, &path, &pathLen,
                                &minorVersion, headers, &numHeaders, lastLen);
        if (status > 0) {
            size_t contentLength = 0;
            for (size_t i = 0; i < numHeaders; i++) {
                if ((headers[i].name_len == strlen("content-length")) && 
                    (0 == strncasecmp(headers[i].name, "content-length", strlen("content-length")))){
                    contentLength = atoi(headers[i].value);
                    break;
                }
            }
            if (contentLength > 0){
                if (con->length == status + static_cast<int32_t>(contentLength)) {
                    //body parse
                    RuruAddress address;
                    bool ret = BodyParse((const char *)(con->data + status), static_cast<int32_t>(contentLength), address);
                    if (ret){
                        std::unique_ptr<RuruClient> client(new RuruClient(&RuruLoop::dtsCtx_, address, udpfd_));
                        AttachClientInfo(client.get());
                        clientMgr_.push_back(std::move(client));
                        std::string response("HTTP/1.1 200 OK\r\n"
                                            "Content-Type: application/json\r\n"
                                            "Connection: close\r\n"
                                            "\r\n");
                        SocketWrite(con->fd, response.c_str(), response.length());
                    }
                    else{
                        SocketWrite(con->fd, HTTP_BAD_REQUEST, strlen(HTTP_BAD_REQUEST));
                    }
                    
                    close(con->fd);
                    delete con;
                    return;
                }
            }
        }
        else if (status == -1){
            close(con->fd);
            delete con;
            return;
        }
        else{
            if (con->length == HTTP_MAX_SIZE){
                SocketWrite(con->fd, HTTP_BAD_REQUEST, strlen(HTTP_BAD_REQUEST));
                close(con->fd);
                delete con;
                return;
            }
        }
    }
}

bool RuruLoop::BodyParse(const char *data, int32_t length, RuruAddress &address)
{
    if (data == NULL || length < 0){
        return false;
    }
    cJSON *root = cJSON_Parse(data);

    if (root == NULL){
        return false;
    }

    cJSON *ip = cJSON_GetObjectItem(root, "ip");
    cJSON *port = cJSON_GetObjectItem(root, "port");
    if (ip == NULL || port == NULL){
        cJSON_Delete(root);
        return false;
    }

    if (!cJSON_IsString(ip) || !cJSON_IsNumber(port)){
        cJSON_Delete(root);
        return false;
    }
    address.host = ntohl((uint32_t)inet_addr(ip->valuestring));
    address.port = port->valueint;

    cJSON_Delete(root);

    return true;
}