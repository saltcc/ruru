#include <stdio.h>
#include "../RuruLoop.h"

int main()
{
    const uint8_t *ip = (uint8_t *)"192.168.28.128";
    const uint8_t *port = (uint8_t *)"9999";
    RuruLoop ruru(ip, port);

    ruru.Loop();
    return 0;
}
