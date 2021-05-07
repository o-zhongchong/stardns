#include "dns_server.h"
#include "dns_resolver.h"

int main(int argc, char** argv)
{
    DNSServer* server = new DNSServer("0.0.0.0", 53);
    server->Start();
	
	DNSResolver* resolver = new DNSResolver();
    
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
                DNSAnswer* ans = resolver->Resolve(msg->questions[i]);
				
				if(ans != nullptr)
				{
					msg->header->answer_count += 1;
					msg->answers.push_back(ans);
				}
            }
            
            server->tx_queue.push(msg);
        }
    }
    
    delete server;
    delete resolver;
    return 0;
}
