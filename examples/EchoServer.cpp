#include <stdio.h>
#include "../RuruLoop.h"

void Echo2Client(RuruEvent &evt)
{
    RuruClient *client = evt.client;

    RuruSctpMessage msg;
    msg.data = evt.data;
    msg.len = evt.length;
    msg.param.sid = evt.param.sid;
    msg.param.ppid = 0;
    
    client->sctpTransport.SendUsrSctpData(&msg);
}

void SendClientCacheData(RuruEvent &evt)
{
    if (evt.client){
        evt.client->sctpTransport.SendSctpCacheData();
    }

    return;
}

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
                    Echo2Client(evt);
                    break;
                case EVT_SendCacheData:
                    SendClientCacheData(evt);
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}
