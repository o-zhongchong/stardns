#include "dns_resolver.h"

DNSResolver::DNSResolver()
{
	//初始化数据库对象
    database = new MySQLDB("127.0.0.1", 3306, "stardns", "", "stardns");
}

DNSResolver::~DNSResolver()
{
	//销毁数据库对象
	if(database != nullptr)
	{
		delete database;
	}
}

DNSAnswer* DNSResolver::Resolve(DNSQuery* query)
{
	//空指针直接返回
	if(query == nullptr)
	{
		return nullptr;
	}
	
	//根据查询域名类型分配给对应的解析器
	switch(query->query_type)
	{
		case 1:
			//调用A记录解析器
			return ResolveA(query);
			break;
		
		case 5:
			//调用CNAME记录解析器
			break;
		
		case 28:
			//调用AAAA记录解析器
			break;
	}
	
	return nullptr;
}

DNSAnswer* DNSResolver::ResolveA(DNSQuery* query)
{
	if(query == nullptr)
	{
		return nullptr;
	}
	
	//生成查询语句
	string sql = string("select record_name,ttl,ip_addr from res_record_a ") + 
                 string("where record_name=\'") + query->query_name + string("\';");	 
	database->Init();
	database->Exec(sql);
	
	//定义num_rows为结果行, num_fields为字段长度
	int num_rows = 0;
	
	if(database->result != NULL)
	{
		//获取查询结果数据
		num_rows = mysql_num_rows(database->result);
	}
	
	if(num_rows == 0)
	{
		//未查询到A记录, 返回空
		return nullptr;
	}
	
	//读取SQL结果行
	database->row = mysql_fetch_row(database->result);
	
	//初始化回答对象
	DNSAnswer* ans = new DNSAnswer();
	ans->query_name  = query->query_name;
	ans->query_type  = query->query_type;
	ans->query_class = query->query_class;
	
	//初始化TTL和资源记录数据
	ans->ttl = stoul( string(database->row[1]) );
	ans->len = 4;
	ans->data = new char[ans->len];
	
	
	unsigned int ipaddr = stoul( string(database->row[2]) );
	ans->data[0] = (ipaddr & 0xFF000000) >> 24;
	ans->data[1] = (ipaddr & 0x00FF0000) >> 16;
	ans->data[2] = (ipaddr & 0x0000FF00) >> 8;
	ans->data[3] = (ipaddr & 0x000000FF);
	
	return ans;
}

DNSAnswer* DNSResolver::ResolveCNAME(DNSQuery* query)
{
	if(query == nullptr)
	{
		return nullptr;
	}
	
	return nullptr;
}

DNSAnswer* DNSResolver::ResolveAAAA(DNSQuery* query)
{
	if(query == nullptr)
	{
		return nullptr;
	}
	
	return nullptr;
}
