#include "dns_answer.h"

DNSAnswer::DNSAnswer()
{
    //资源记录数据初始化
	len = 0;
    data = nullptr;
}

DNSAnswer::~DNSAnswer()
{
    //回收资源数据对象
    delete data;
    data = nullptr;
}
