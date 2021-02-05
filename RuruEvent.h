#pragma once
#include "RuruClient.h"
#include <stdint.h>

enum RuruEventType
{
    EVT_None,
    EVT_RecvClientData,
    EVT_HeartBeat,
    EVT_SendCacheData
};

struct RuruDataParam
{
    int32_t sid = 0;
    int32_t ssn = 0;
    int32_t tsn = 0;
};

struct RuruEvent
{
    RuruEventType type = EVT_None;
    RuruClient *client = nullptr;
    uint8_t *data = nullptr;
    uint16_t length = 0;
    RuruDataParam param;
};