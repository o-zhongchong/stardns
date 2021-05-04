#include "dns_server.h"

DNSServer::DNSServer(const char* ip, int port)
{
    //创建服务端socket
    svr_sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(svr_sock < 0)
    {
        perror("Failed to create server socket");
        exit(-1);
    }
    
    //服务端地址初始化
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = inet_addr(ip);
    svr_addr.sin_port = htons(port);
    
    //绑定服务端地址
    if(bind(svr_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) < 0)
    {
        perror("Faild to bind server address");
        exit(-1);
    }
    
    //工作线程状态初始化
    thread_running = false;
}

DNSServer::~DNSServer()
{
    //关闭服务端socket
    close(svr_sock);
    
    //释放请求队列资源
    while(!rx_queue.empty())
    {
        DNSMessage* p = rx_queue.front();
        rx_queue.pop();
        delete p;
    }
    
    //释放响应队列资源
    while(!tx_queue.empty())
    {
        DNSMessage* p = tx_queue.front();
        tx_queue.pop();
        delete p;
    }
}

int DNSServer::Start()
{
    if(!thread_running)
    {
        //创建工作线程
        thread_running = true;
        
        if( pthread_create(&thread_id, NULL, DNSServer::Work, this) )
        {
            //线程创建失败
            thread_running = false;
            thread_id = 0;
        }
    }
    
    return 0;
}
    
int DNSServer::Stop()
{
    if(thread_running)
    {
        //停止工作线程
        thread_running = false;
        pthread_join(thread_id, NULL);
    }
    
    return 0;
}

void* DNSServer::Work(void* arg)
{
    DNSServer* server = (DNSServer*)arg;
    int max_buff_size = 4096;
    char* buff = new char[max_buff_size];
    int recv_len = 0;
    
    while(server->thread_running)
    {
        //解析DNS报文, 放入请求队列
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        recv_len = recvfrom(server->svr_sock, buff, max_buff_size, 0, (struct sockaddr*)&client_addr,
                            &client_addr_len);
        
        if(recv_len > 0)
        {
            //创建DNS报文对象
            DNSMessage* msg = new DNSMessage();
            msg->client_addr = client_addr;
            msg->ip = inet_ntoa(client_addr.sin_addr);
            msg->port = ntohs(client_addr.sin_port);
            int pos = 0;
            
            if( GetHeader(buff, recv_len, pos, msg->header) == 0 )
            {
                //DNS报文头部解析成功, 开始解析DNS查询段
                for(int i = 0; i < msg->header->question_count; ++i)
                {
                    DNSQuery* query = new DNSQuery();
                    
                    if( GetQuestion(buff, recv_len, pos, query) == 0 )
                    {
                        //DNS报文查询段解析成功，加入DNS报文对象
                        msg->questions.push_back(query);
                    }
                    else
                    {
                        //DNS报文查询段解析失败
                        delete query;
                        delete msg;
                        query = nullptr;
                        msg = nullptr;
                        continue;
                    }
                }
            }
            else
            {
                //DNS报文头部解析失败
                delete msg;
                msg = nullptr;
                continue;
            }
            
            //DNS报文解析成功, 加入请求队列
            server->rx_queue.push(msg);
        }
    }
}

int DNSServer::GetHeader(const char* buff, int len, int &pos, DNSHeader* header)
{
    //头部字节数小于12字节，解析失败
    if(pos + 12 >= len)
    {
        return -1;
    }
    
    //解析会话ID
    header->trans_id = buff[pos + 0];
    header->trans_id <<= 8;
    header->trans_id = header->trans_id | buff[pos + 1];
    
    //解析报文参数
    header->flags = buff[pos + 2];
    header->flags <<= 8;
    header->flags = header->flags | buff[pos + 3];
    
    //解析请求数量
    header->question_count = buff[pos + 4];
    header->question_count <<= 8;
    header->question_count = header->question_count | buff[pos + 5];
    
    //解析应答数量
    header->answer_count = buff[pos + 6];
    header->answer_count <<= 8;
    header->answer_count = header->answer_count | buff[pos + 7];
    
    //解析授权应答数量
    header->authority_rr_count = buff[pos + 8];
    header->authority_rr_count <<= 8;
    header->authority_rr_count = header->authority_rr_count | buff[pos + 9];
    
    //解析附加内容数量
    header->additional_rr_count = buff[pos + 10];
    header->additional_rr_count <<= 8;
    header->additional_rr_count = header->additional_rr_count | buff[pos + 11];
    
    pos += 12;
    return 0;
}

int DNSServer::GetQuestion(const char* buff, int len, int &pos, DNSQuery* query)
{
    //查询域名初始化
    query->query_name = "";
    
    //解析查询域名
    while(pos < len && buff[pos] != 0)
    {
        if(query->query_name != "")
        {
            query->query_name.push_back('.');
        }
        
        for(int i = 0; i < (int)buff[pos]; ++i)
        {
            query->query_name.push_back(buff[pos+1+i]);
        }
        
        pos += (int)buff[pos] + 1;
    }
    
    if(query->query_name == "" || pos + 4 >= len)
    {
        return -1;
    }
    
    //解析查询类型
    query->query_type = buff[pos + 1];
    query->query_type <<= 8;
    query->query_type = query->query_type | buff[pos + 2];
    
    //解析查询类
    query->query_class = buff[pos + 3];
    query->query_class <<= 8;
    query->query_class = query->query_class | buff[pos + 4];
    
    pos += 5;
    return 0;
}

