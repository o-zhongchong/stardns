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

int MySQLDB::Connect()
{
    //建立到数据库的连接
    conn = mysql_init(NULL);
    
    //数据库初始化失败
    if(conn == NULL)
    {
        printf("MySQL database init failed. Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return -1;
    }
    
    //创建数据库连接
    conn = mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 
                              port, NULL, 0);
    
    //数据库连接失败
    if(conn == NULL)
    {
        printf("MySQL database connection failed. Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        return -1;
    }
    
    return 0;
}

int MySQLDB::Exec(string sql)
{
    //数据库连接为空
    if(conn == NULL)
    {
        printf("MySQL database connection does not exist\n");
        return -1;
    }
    
    //执行SQL语句
    if( mysql_query(conn, sql.c_str()) )
    {
        string err = "Failed to exec sql \"" + sql + "\"";
        printf("%s\n", err.c_str());
        return -1;
    }
    
    //获取执行结果
    result = mysql_store_result(conn);
    
    //执行结果获取失败
    if(result == NULL)
    {
        string err = "Failed to get sql result \"" + sql + "\"";
        printf("%s\n", err.c_str());
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
