#ifndef __DNS_QUERY_H__
#define __DNS_QUERY_H__

#include <string>

using std::string;

class DNSQuery
{
public:
    DNSQuery();
    virtual ~DNSQuery();
    
public:
    //DNS报文查询域名
    string query_name;
    
    //DNS报文查询类型
    unsigned short query_type;
    
    //DNS报文查询类
    unsigned short query_class;
};

#endif
