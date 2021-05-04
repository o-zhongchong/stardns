#ifndef __DNS_ANSWER_H__
#define __DNS_ANSWER_H__

#include <string>

using std::string;

class DNSAnswer
{
public:
    DNSAnswer();
    virtual ~DNSAnswer();
    
public:
    //DNS报文查询域名
    string query_name;
    
    //DNS报文查询类型
    unsigned short query_type;
    
    //DNS报文查询类
    unsigned short query_class;
    
    //生存时间
    int ttl;
    
    //资源记录长度
    unsigned short len;
    
    //资源记录数据
    char* data;
};

#endif
