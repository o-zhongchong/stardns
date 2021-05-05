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
    
    //DNS报文头部
    DNSHeader* header;
    
    //DNS报文查询问题数
    vector<DNSQuery*> questions;
    
    //DNS报文回答数
    vector<DNSAnswer*> answers;
    
    //DNS报文授权回答数
    vector<DNSAnswer*> authority_answers;
    
    //DNS报文附加内容数
    vector<DNSAnswer*> additional_answers;
};

#endif
