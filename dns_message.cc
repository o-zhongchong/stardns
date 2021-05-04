#include "dns_message.h"

DNSMessage::DNSMessage()
{
    //DNS报文请求头初始化
    header = new DNSHeader();
}

DNSMessage::~DNSMessage()
{
    //释放DNS报文请求头对象资源
    delete header;
    header = nullptr;
    
    //释放查询对象资源
    if(!questions.empty())
    {
        for(DNSQuery* p : questions)
        {
            delete p;
            p = nullptr;
        }
        
        questions.clear();
    }
    
    //释放应答对象资源
    if(!answers.empty())
    {
        for(DNSAnswer* p : answers)
        {
            delete p;
            p = nullptr;
        }
        
        answers.clear();
    }
    
    //释放权威应答对象资源
    if(!authority_answers.empty())
    {
        for(DNSAnswer* p : authority_answers)
        {
            delete p;
            p = nullptr;
        }
        
        authority_answers.clear();
    }
    
    //释放附加应答对象资源
    if(!additional_answers.empty())
    {
        for(DNSAnswer* p : additional_answers)
        {
            delete p;
            p = nullptr;
        }
        
        additional_answers.clear();
    }
}
