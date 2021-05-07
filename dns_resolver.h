#ifndef __DNS_RESOLVER_H__
#define __DNS_RESOLVER_H__

#include <cstdlib>
#include "dns_query.h"
#include "dns_answer.h"
#include "mysql_db.h"

class DNSResolver
{
public:
    DNSResolver();
    virtual ~DNSResolver();
	DNSAnswer* Resolve(DNSQuery* query);
	
protected:
	DNSAnswer* ResolveA(DNSQuery* query);
	DNSAnswer* ResolveCNAME(DNSQuery* query);
	DNSAnswer* ResolveAAAA(DNSQuery* query);

protected:
	MySQLDB* database;
};

#endif
