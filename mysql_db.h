#ifndef __MYSQL_DB_H__
#define __MYSQL_DB_H__

#include <string>
#include <mysql.h>

using std::string;

class MySQLDB
{
public:
    MySQLDB(string host, int port, string user, string passwd, string db);
    virtual ~MySQLDB();
	int Init();
	int Exec(string sql);
	int FreeResult();
	
public:
	//MySQL数据库对象
	MYSQL *conn;
	MYSQL_RES *result;
    MYSQL_ROW row;
	
	//数据库连接串
	string host;
	int port;
	string user;
	string passwd;
	string db;
};

#endif
