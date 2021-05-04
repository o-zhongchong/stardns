#include "dns_header.h"

DNSHeader::DNSHeader()
{
    
}

DNSHeader::~DNSHeader()
{
    
}

int DNSHeader::GetFlags()
{
	//从DNS报文中请求头参数字段中解析出详细参数
	flags_qr = flags & 0x8000;
	flags_qr >>= 15;
	
	flags_opcode = flags & 0x7800;
	flags_opcode >>= 11;
	
	flags_aa = flags & 0x0400;
	flags_aa >>= 10;
	
	flags_tc = flags & 0x0200;
	flags_tc >>= 9;
	
	flags_rd = flags & 0x0100;
	flags_rd >>= 8;
	
	flags_ra = flags & 0x0080;
	flags_ra >>= 7;
	
	flags_z = flags_z & 0x0070;
	flags_z >>= 4;
	
	flags_rcode = flags & 0x000F;
	return 0;
}

int DNSHeader::SetFlags()
{
	//封装详细参数到DNS报文中请求头参数字段
	flags = flags & 0x0000;
	flags = flags | ( flags_qr << 15 );
	flags = flags | ( flags_opcode << 11);
	flags = flags | ( flags_aa << 10);
	flags = flags | ( flags_tc << 9);
	flags = flags | ( flags_rd << 8);
	flags = flags | ( flags_ra << 7);
	flags = flags | ( flags_z << 4);
	flags = flags | flags_rcode;
	return 0;
}
