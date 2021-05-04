#include "dns_server.h"

int main(int argc, char** argv)
{
    DNSServer* server = new DNSServer("0.0.0.0", 53);
    server->Start();
    
    while(1)
    {
        while(!server->rx_queue.empty())
        {
            DNSMessage* msg = server->rx_queue.front();
            server->rx_queue.pop();
            printf("收到客户端 %s:%d 发来的请求\n", msg->ip.c_str(), msg->port);
            delete msg;
            msg = nullptr;
        }
    }
    
    delete server;
    server = nullptr;
    return 0;
}
