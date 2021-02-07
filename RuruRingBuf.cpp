#include "RuruRingBuf.h"
#include <string.h>
template <typename T>
RuruRingBuf<T>::RuruRingBuf(uint32_t cap)
{
    this->cap = cap;
    this->rd = 0;
    this->wt = 0;
    this->memory = new T[this->cap];
}

template <typename T>
RuruRingBuf<T>::~RuruRingBuf()
{
    if (memory){
        delete []memory;
        memory = nullptr;
    }
}

template <typename T>
void RuruRingBuf<T>::RingBufReset()
{
    rd = 0;
    wt = 0;
}

template <typename T>
bool RuruRingBuf<T>::RingBufEmpty()
{
    return rd == wt;
}

template <typename T>
bool RuruRingBuf<T>::RingBufRead(T &out)
{
    if (wt != rd){
        memcpy(&out, &(memory[rd]), sizeof(T));
        rd = (rd + 1) % cap;
        return true;
    }

    return false;
}

template <typename T>
bool RuruRingBuf<T>::RingBufWrite(T &input)
{
    if (rd != ((wt + 1) % cap)){
        memcpy(&(memory[wt]), &input, sizeof(T));
        wt = (wt + 1) % cap;
        return true;
    }

    return false;
}