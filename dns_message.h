#ifndef __DNS_MESSAGE_H__
#define __DNS_MESSAGE_H__

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <vector>
#include "dns_header.h"
#include "dns_query.h"
#include "dns_answer.h"

using std::string;
using std::vector;

class DNSMessage
{
public:
    DNSMessage();
    virtual ~DNSMessage();
    
public:
    //客户端地址
    struct sockaddr_in client_addr;
    string ip;
    int port;
    
    //报文头部
    DNSHeader* header;
    
    //Questions
    vector<DNSQuery*> questions;
    
    //Answer RRs
    vector<DNSAnswer*> answers;
    
    //Authority RRs
    vector<DNSAnswer*> authority_answers;
    
    //Additional
    vector<DNSAnswer*> additional_answers;
};

#endif
