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
    //停止工作线程
    Stop();
    
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
        //启动接收&&发送工作线程
        thread_running = true;
        
        if( pthread_create(&recv_thread_id, NULL, DNSServer::RecvWorker, this) )
        {
            //接收线程创建失败
            thread_running = false;
            recv_thread_id = 0;
        }
        
        if( pthread_create(&send_thread_id, NULL, DNSServer::SendWorker, this) )
        {
            //发送线程创建失败
            thread_running = false;
            send_thread_id = 0;
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
        pthread_join(recv_thread_id, NULL);
        pthread_join(send_thread_id, NULL);
    }
    
    return 0;
}

void* DNSServer::RecvWorker(void* arg)
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
            //调试输出
            printf("收到字节数 %d\n", recv_len);
            
            //创建DNS报文对象
            DNSMessage* msg = new DNSMessage();
            msg->client_addr = client_addr;
            msg->ip = inet_ntoa(client_addr.sin_addr);
            msg->port = ntohs(client_addr.sin_port);
            int pos = 0;
            
            if( GetHeader(buff, recv_len, pos, msg->header) == 0 )
            {
                //DNS报文头部解析成功, 开始解析DNS问题区域
                for(int i = 0; i < msg->header->question_count; ++i)
                {
                    DNSQuery* query = new DNSQuery();
                    
                    if( GetQuestion(buff, recv_len, pos, query) == 0 )
                    {
                        //DNS报文问题区域解析成功，放入DNS报文对象
                        msg->questions.push_back(query);
                    }
                    else
                    {
                        //DNS报文问题区域解析失败
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
    
    return NULL;
}

void* DNSServer::SendWorker(void* arg)
{
    DNSServer* server = (DNSServer*)arg;
    
    while(server->thread_running)
    {
        //从发送队列读取任务, 回包给客户端
        while(!server->tx_queue.empty())
        {
            //取出发送队列任务
            DNSMessage* msg = server->tx_queue.front();
            server->tx_queue.pop();
            
            //计算报文长度, 创建发送缓冲区
            int max_buff_size = GetMessageLength(msg);
            char* buff = new char[max_buff_size];
            int pos = 0;
            
            //调试输出
            printf("响应报文字节数%d\n", max_buff_size);
            
            //填充DNS报文头到发送缓冲区
            SetHeader(buff, max_buff_size, pos, msg->header);
            
            //填充DNS报文问题区域到发送缓冲区
            for(int i=0; i < (int)msg->questions.size(); ++i)
            {
                SetQuestion(buff, max_buff_size, pos, msg->questions[i]);
            }
            
            //填充DNS报文回答区域到发送缓冲区
            for(int i=0; i < (int)msg->answers.size(); ++i)
            {
                SetAnswer(buff, max_buff_size, pos, msg->answers[i]);
            }
            
            //填充DNS报文回答区域到发送缓冲区
            for(int i=0; i < (int)msg->authority_answers.size(); ++i)
            {
                SetAnswer(buff, max_buff_size, pos, msg->authority_answers[i]);
            }
            
            //填充DNS报文回答区域到发送缓冲区
            for(int i=0; i < (int)msg->additional_answers.size(); ++i)
            {
                SetAnswer(buff, max_buff_size, pos, msg->additional_answers[i]);
            }
            
            //发送报文给客户端
            socklen_t client_addr_len = sizeof(msg->client_addr);
            sendto(server->svr_sock, buff, max_buff_size, 0, (struct sockaddr*)&msg->client_addr, 
                   client_addr_len);
            
            delete msg;
            delete buff;
            msg = nullptr;
            buff = nullptr;
        }
    }
    
    return NULL;
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
    
    //解析DNS报文参数
    header->flags = buff[pos + 2];
    header->flags <<= 8;
    header->flags = header->flags | buff[pos + 3];
    header->GetFlags();
    
    //解析DNS报文问题数量
    header->question_count = buff[pos + 4];
    header->question_count <<= 8;
    header->question_count = header->question_count | buff[pos + 5];
    
    //解析DNS报文回答数量
    header->answer_count = buff[pos + 6];
    header->answer_count <<= 8;
    header->answer_count = header->answer_count | buff[pos + 7];
    
    //解析DNS报文授权回答数量
    header->authority_rr_count = buff[pos + 8];
    header->authority_rr_count <<= 8;
    header->authority_rr_count = header->authority_rr_count | buff[pos + 9];
    
    //解析DNS报文附加内容数量
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
    
    //解析查询域名失败 or 问题区域不完整
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

int DNSServer::GetMessageLength(DNSMessage* msg)
{
    if(msg == nullptr)
    {
        return 0;
    }
    
    //初始化DNS报文头部字节长度
    int len = 12;
    
    //计算DNS报文问题区域字节数
    for(unsigned short i = 0; i < msg->questions.size(); ++i)
    {
        DNSQuery* p = msg->questions[i];
        
        //查询域名字段长度
        len += p->query_name.size() + 2;
        
        //查询类型和查询类字段
        len += 4;
    }
    
    //计算DNS报文回答字节数
    for(unsigned short i = 0; i < msg->answers.size(); ++i)
    {
        DNSAnswer* p = msg->answers[i];
        
        //查询域名字段长度
        len += p->query_name.size() + 2;
        
        //查询类型和查询类字段
        len += 4;
        
        //TTL字段长度
        len += 4;
        
        //资源记录字段长度
        len += p->len + 2;
    }
    
    //计算DNS报文授权回答字节数
    for(unsigned short i = 0; i < msg->authority_answers.size(); ++i)
    {
        DNSAnswer* p = msg->authority_answers[i];
        
        //查询域名字段长度
        len += p->query_name.size() + 2;
        
        //查询类型和查询类字段
        len += 4;
        
        //TTL字段长度
        len += 4;
        
        //资源记录字段长度
        len += p->len + 2;
    }
    
    //计算DNS报文附加内容字节数
    for(unsigned short i = 0; i < msg->additional_answers.size(); ++i)
    {
        DNSAnswer* p = msg->additional_answers[i];
        
        //查询域名字段长度
        len += p->query_name.size() + 2;
        
        //查询类型和查询类字段
        len += 4;
        
        //TTL字段长度
        len += 4;
        
        //资源记录字段长度
        len += p->len + 2;
    }
    
    return len;
}

int DNSServer::SetHeader(char* buff, int len, int &pos, DNSHeader* header)
{
    //发送缓冲区可用字节数小于12字节，填充失败
    if(pos + 12 >= len)
    {
        return -1;
    }
    
    //填充DNS报文头部字段到发送缓冲区
    buff[pos++] = (header->trans_id & 0xFF00) >> 8;
    buff[pos++] = (header->trans_id & 0x00FF);
    
    buff[pos++] = (header->flags & 0xFF00) >> 8;
    buff[pos++] = (header->flags & 0x00FF);
    
    buff[pos++] = (header->question_count & 0xFF00) >> 8;
    buff[pos++] = (header->question_count & 0x00FF);
    
    buff[pos++] = (header->answer_count & 0xFF00) >> 8;
    buff[pos++] = (header->answer_count & 0x00FF);
    
    buff[pos++] = (header->authority_rr_count & 0xFF00) >> 8;
    buff[pos++] = (header->authority_rr_count & 0x00FF);
    
    buff[pos++] = (header->additional_rr_count & 0xFF00) >> 8;
    buff[pos++] = (header->additional_rr_count & 0x00FF);
    
    return 0;
}

int DNSServer::SetQuestion(char* buff, int len, int &pos, DNSQuery* query)
{
    //查询域名按点号分割成字符串数组
    vector<string> str_split;
    int qname_len = query->query_name.size();
    
    for(int i=0, j=0; i < qname_len; ++i)
    {
        if(query->query_name[i] == '.')
        {
            if(i - j > 0)
            {
                string t = query->query_name.substr(j, i - j);
                str_split.push_back(t);
            }

            j = i + 1;
        }
        else if(query->query_name[i] != '.' && i == qname_len - 1)
        {
            if(qname_len - j > 0)
            {
                string t = query->query_name.substr(j, qname_len - j);
                str_split.push_back(t);
            }
            
            j = i + 1;
        }
    }
    
    //填充查询域名到发送缓冲区
    for(string &t : str_split)
    {
        buff[pos++] = (char)t.size();
        
        for(int i=0; i < buff[pos - 1]; ++i)
        {
            buff[pos + i] = t[i];
        }
        
        pos += (char)t.size();
    }
    
    //查询域名末尾填充0
    buff[pos++] = 0;
    
    //填充查询类型
    buff[pos++] = (query->query_type & 0xFF00) >> 8;
    buff[pos++] = (query->query_type & 0x00FF);
    
    //填充查询类
    buff[pos++] = (query->query_class & 0xFF00) >> 8;
    buff[pos++] = (query->query_class & 0x00FF);
    return 0;
}

int DNSServer::SetAnswer(char* buff, int len, int &pos, DNSAnswer* answer)
{
    //查询域名按点号分割成字符串数组
    vector<string> str_split;
    int qname_len = answer->query_name.size();
    
    for(int i=0, j=0; i < qname_len; ++i)
    {
        if(answer->query_name[i] == '.')
        {
            if(i - j > 0)
            {
                string t = answer->query_name.substr(j, i - j);
                str_split.push_back(t);
            }
            
            j = i + 1;
        }
        else if(answer->query_name[i] != '.' && i == qname_len - 1)
        {
            if(qname_len - j > 0)
            {
                string t = answer->query_name.substr(j, qname_len - j);
                str_split.push_back(t);
            }
            
            j = i + 1;
        }
    }
    
    //填充查询域名到发送缓冲区
    for(string &t : str_split)
    {
        buff[pos++] = (char)t.size();
        
        for(int i=0; i < buff[pos - 1]; ++i)
        {
            buff[pos + i] = t[i];
        }
        
        pos += (char)t.size();
    }
    
    //查询域名末尾填充0
    buff[pos++] = 0;
    
    //填充查询类型
    buff[pos++] = (answer->query_type & 0xFF00) >> 8;
    buff[pos++] = (answer->query_type & 0x00FF);
    
    //填充查询类
    buff[pos++] = (answer->query_class & 0xFF00) >> 8;
    buff[pos++] = (answer->query_class & 0x00FF);
    
    //填充TTL
    buff[pos++] = (answer->ttl & 0xFF000000) >> 24;
    buff[pos++] = (answer->ttl & 0x00FF0000) >> 16;
    buff[pos++] = (answer->ttl & 0x0000FF00) >> 8;
    buff[pos++] = (answer->ttl & 0x000000FF);
    
    //填充资源记录长度
    buff[pos++] = (answer->len & 0xFF00) >> 8;
    buff[pos++] = (answer->len & 0x00FF);
    
    //填充资源记录数据
    for(int i=0; i < answer->len; ++i)
    {
        buff[pos++] = answer->data[i];
    }
    
    return 0;
}
