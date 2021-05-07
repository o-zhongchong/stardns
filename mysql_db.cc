#include "mysql_db.h"

MySQLDB::MySQLDB(string host, int port, string user, string passwd, string db)
{
	//初始化数据库连接串
	this->host = host;
	this->port = port;
	this->user = user;
	this->passwd = passwd;
	this->db = db;
}

MySQLDB::~MySQLDB()
{
	//关闭数据库连接
    if(conn != NULL) 
    {
        mysql_close(conn);
    }
}

int MySQLDB::Init()
{
	//建立到数据库的连接
	conn = mysql_init(NULL);
	
	if(conn == NULL)
	{
		//数据库初始化失败
		perror("Could not create MySQL database object\n");
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(-1);
	}
	
	conn = mysql_real_connect(conn, host.c_str(), user.c_str(),
							  passwd.c_str(), db.c_str(), 
							  port, NULL, 0);
	
	//数据库连接失败
	if(conn == NULL)
	{
		perror("MySQL database connection failed\n");
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		return -1;
	}
	
	return 0;
}

int MySQLDB::Exec(string sql)
{
	//数据库连接为空
	if(conn == NULL)
	{
		perror("MySQL database connection does not exist\n");
		return -1;
	}
	
	//执行SQL语句
	if( mysql_query(conn, sql.c_str()) )
	{
		string err = "Failed to exec sql \"" + sql + "\"";
		perror(err.c_str());
		return -1;
	}
	
	//获取执行结果
	result = mysql_store_result(conn);
	
	if(result == NULL)
	{
		return -1;
	}
	
	return 0;
}

int MySQLDB::FreeResult()
{
	//释放SQL执行结果对象
	mysql_free_result(result);
	return 0;
}
