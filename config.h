#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>

using std::string;

class Config
{
public:
    Config();
    virtual ~Config();
    
public:
    //MySQL数据库连接串
    string ip;
    int port;
    string user;
    string passwd;
	string db;
    
protected:
    string config_file;
};

#endif
