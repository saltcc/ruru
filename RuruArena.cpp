#include "RuruArena.h"
#include <stdlib.h>

RuruArena::RuruArena(int32_t capacity)
{
    this->capacity = 1 << capacity;
    memory = new uint8_t[this->capacity];
    pos = 0;
    return;
}

RuruArena::~RuruArena()
{
    if (memory){
        delete []memory;
        memory = nullptr;
    }
    return;
}

uint8_t *RuruArena::ArenaAlloc(int32_t blockSize)
{
    if (blockSize <= 0){
        return nullptr;
    }

    int32_t remain = capacity - pos;

    if (remain < blockSize){
        return nullptr;
    }

    uint8_t *mGet = memory + pos;
    pos += blockSize;

    return mGet;
}