#ifndef _SQL_
#define _SQL_

#include<iostream>
#include<string>
#include<mysql.h>
using namespace std;

class SqlApi
{
public:
	SqlApi(const string &_h = "localhost",\
       		const int &_port=3306,\
       		const string &_u="mysql",\
       		const string &_p=".",\
       		const string &_db="person");
//	SqlApi(const string &_h,\
//               const int &_port,\
//               const string &_u,\
//               const string &_p,\
//               const string &_db);
	bool insert(const string &str);
     	
	
	bool connect();

	~SqlApi();
private:
	MYSQL *conn; //链接数据库的句柄
        string host; //链接的主机ip
        string user; //链接的用户名
	string passwd; //密码
        string db;   //要链接的数据库
        int port;    //数据库服务器的端口号	
};

#endif 

