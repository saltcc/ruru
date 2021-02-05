#pragma once

#include <stdint.h>

class RuruArena
{
public:
    RuruArena(int32_t capacity = 20);
    ~RuruArena();

    uint8_t *ArenaAlloc(int32_t blockSize);
    void ArenaReset(){pos = 0;}
private:
    uint8_t *memory;
    int32_t pos;
    int32_t capacity;
};