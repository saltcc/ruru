#pragma once
#include <stdint.h>

template <typename T>
class RuruRingBuf
{
public:
    RuruRingBuf(uint32_t);
    ~RuruRingBuf();
    void RingBufReset();
    bool RingBufEmpty();
    bool RingBufRead(T &out);
    bool RingBufWrite(T &input);
    T *memory;
    uint32_t cap;
    uint32_t rd;
    uint32_t wt;
};