#pragma once

#include <stdint.h>

struct RuruAddress
{
    uint32_t host;
    uint16_t port;
};

int32_t CreateTcpSocket(const uint8_t *port);
int32_t CreateUdpSocket(const uint8_t *port);
int32_t SetNonBlocking(int32_t sfd);
