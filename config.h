#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>

using std::string;

class Config
{
public:
    Config(string filepath);
    virtual ~Config();
	int LoadConfig();
	
protected:
    string filepath;
	unordered_map<string, int> config_map1;
	unordered_map<string, string> config_map2;
};

#endif
