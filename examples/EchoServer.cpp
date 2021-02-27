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

void SendHeartBeat(RuruEvent &evt)
{
    if (evt.client){
        evt.client->ClientSendHeartBeatData();
    }
}

int main(int argc, char **argv)
{
    if (argc <= 2){
        std::cerr<<"./EchoServer localip lostport"<<std::endl;
        return 0;
    }
    const char *ip = argv[1];
    const char *port = argv[2];
    RuruLoop ruru(ip, port);

    for (;;){
        RuruEvent evt;
        while (ruru.Loop(evt)){
            switch (evt.type){
                case EVT_HeartBeat:
                    SendHeartBeat(evt);
                    printf("send heart beat\n");
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
