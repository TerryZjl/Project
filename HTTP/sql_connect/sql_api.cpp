#include"sql_api.h"

SqlApi::SqlApi(const string &_h,\
       const int &_port,\
       const string &_u,\
       const string &_p,\
       const string &_db)
{
	if((conn = mysql_init(NULL))==NULL){
		cout<<"mysql_init error!!!"<<endl;
	}
	host = _h;
	user = _u;
	passwd = _p;
	db = _db;
	port = _port;
}

bool SqlApi::connect()
{	
	bool ret = true;
	if(mysql_real_connect(conn,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,NULL, 0)==NULL){
		printf("connect");
		ret = false;
	}
	return ret;
}
bool  SqlApi::insert(const string &str)
{	
	bool ret = true;
	string sql = "INSERT INTO person (name,age,sex,phone) VALUES (";
	sql += str;
	sql += ")";
	if(mysql_query(conn,sql.c_str()) != 0){
		perror("mysql_query!");
		ret = false;
	}
	cout<<"ret:"<<ret<<endl;
	return ret;
}
