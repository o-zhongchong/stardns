#ifndef __DNS_HEADER_H__
#define __DNS_HEADER_H__

class DNSHeader
{
public:
    DNSHeader();
    virtual ~DNSHeader();
    int GetFlags();
    int SetFlags();
    
public:
    //请求ID
    unsigned short trans_id;
    
    //报文参数
    unsigned short flags;
    
    //报文参数:查询/响应的标志位, 0=查询, 1=响应
    unsigned short flags_qr;
    
    //报文参数:查询/响应类型, 0=标准查询, 1=反向查询, 2=服务器状态查询
    unsigned short flags_opcode;
    
    //报文参数:授权回答标志位
    unsigned short flags_aa;
    
    //报文参数:截断标志位
    unsigned short flags_tc;
    
    //报文参数:请求递归查询标志位
    unsigned short flags_rd;
    
    //报文参数:响应递归回答标志位
    unsigned short flags_ra;
    
    //报文参数:保留字段
    unsigned short flags_z;
    
    //报文参数:返回码
    unsigned short flags_rcode;
    
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
