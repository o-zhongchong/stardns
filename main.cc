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
            
            if(msg->header->flags_qr != 0)
            {
                delete msg;
                msg = nullptr;
                continue;
            }
            
            msg->header->flags_qr = 1;
            msg->header->flags_ra = 0;
            msg->header->flags_rcode = 0;
            msg->header->SetFlags();
            msg->header->answer_count = 0;
            
            for(int i=0; i < msg->header->question_count; ++i)
            {
                if(msg->questions[i]->query_type != 1)
                {
                    continue;
                }
                
                msg->header->answer_count += 1;
                DNSAnswer* ans = new DNSAnswer();
                ans->query_name = msg->questions[i]->query_name;
                ans->query_type = msg->questions[i]->query_type;
                ans->query_class = msg->questions[i]->query_class;
                
                ans->ttl = 600;
                ans->len = 4;
                ans->data = new char[ans->len];
                ans->data[0] = 0;
                ans->data[1] = 0;
                ans->data[2] = 0;
                ans->data[3] = 1;
                
                msg->answers.push_back(ans);
            }
            
            server->tx_queue.push(msg);
        }
    }
    
    delete server;
    server = nullptr;
    return 0;
}
