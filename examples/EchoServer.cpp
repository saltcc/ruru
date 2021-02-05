#include <stdio.h>
#include "../RuruLoop.h"

int main()
{
    const uint8_t *ip = (uint8_t *)"192.168.28.128";
    const uint8_t *port = (uint8_t *)"9999";
    RuruLoop ruru(ip, port);

    for (;;){
        RuruEvent evt;
        while (ruru.Loop(evt)){
            switch (evt.type){
                case EVT_HeartBeat:
                    break;
                case EVT_RecvClientData:
                    printf("recv data : %s\n", evt.data);
                    break;
                case EVT_SendCacheData:
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}
