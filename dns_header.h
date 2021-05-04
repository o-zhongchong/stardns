#ifndef __DNS_HEADER_H__
#define __DNS_HEADER_H__

class DNSHeader
{
public:
    DNSHeader();
    virtual ~DNSHeader();
    
public:
    //请求ID
    unsigned short trans_id;
    
    //报文参数
    unsigned short flags;
    
    //查询请求数量
    unsigned short question_count;
    
    //回答资源记录数
    unsigned short answer_count;
    
    //权威应答资源记录数
    unsigned short authority_rr_count;
    
    //附加资源记录数
    unsigned short additional_rr_count;
};

#endif
