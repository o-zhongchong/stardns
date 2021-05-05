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
    static void* RecvWorker(void* arg);
    static void* SendWorker(void* arg);
    
    //DNS报文头部解析函数
    static int GetHeader(const char* buff, int len, int &pos, DNSHeader* header);
    
    //DNS报文问题区域解析函数
    static int GetQuestion(const char* buff, int len, int &pos, DNSQuery* query);
    
    //计算DNS报文对象长度
    static int GetMessageLength(DNSMessage* msg);
    
    //DNS报文头部填充函数
    static int SetHeader(char* buff, int len, int &pos, DNSHeader* header);
    
    //DNS报文问题区域填充函数
    static int SetQuestion(char* buff, int len, int &pos, DNSQuery* query);
    
    //DNS报文回答区域填充函数
    static int SetAnswer(char* buff, int len, int &pos, DNSAnswer* answer);
    
public:
    //DNS请求队列
    queue<DNSMessage*> rx_queue;
    
    //DNS发送队列
    queue<DNSMessage*> tx_queue;
    
protected:
    //工作线程
    pthread_t recv_thread_id;
    pthread_t send_thread_id;
    bool thread_running;
    
    //服务端socket
    int svr_sock;
    
    //服务端地址
    struct sockaddr_in svr_addr;
};

#endif
