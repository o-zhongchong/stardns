#ifndef __DNS_SERVER_H__
#define __DNS_SERVER_H__

#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <queue>
#include "dns_message.h"

using std::queue;

class DNSServer
{
public:
    DNSServer(const char* ip, int port);
    virtual ~DNSServer();
    int Start();
    int Stop();
    
protected:
    //工作线程解析DNS报文
    static void* Work(void* arg);
    
    //DNS报文头部解析函数
    static int GetHeader(const char* buff, int len, int &pos, DNSHeader* msg);
    
    //DNS报文查询段解析函数
    static int GetQuestion(const char* buff, int length, int &pos, DNSQuery* query);

public:
    //DNS请求队列
    queue<DNSMessage*> rx_queue;
    
    //DNS响应队列
    queue<DNSMessage*> tx_queue;
    
protected:
    //工作线程
    pthread_t thread_id;
    bool thread_running;
    
    //服务端socket
    int svr_sock;
    
    //服务端地址
    struct sockaddr_in svr_addr;
};

#endif
