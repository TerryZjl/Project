#include"sql_api.h"


SqlApi::SqlApi(const string &_h = "localhost",\
       const int &_port=3306,\
       const string &_u="mysql",\
       const string &_p=".",\
       const string &_db="person")
{
	if(conn = mysql_init(NULL)==NULL){
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
	if(mysql_real_connect(conn,host.c_str(),user.c_str(),passwd.c_str(),port,NULL, 0)==NULL){
		perror("connect");
		ret = false;
	}
	return ret;
}

bool SqlApi::select()
{
	string sql = "SELECT * form person"		
}
bool  SqlApi::insert(const string &str)
{	
	bool ret = true;
	string sql = "INSERT INTO person (name,age,sex,phone) VALUES (";
	sql += _name;
	sql += ")";
	if(mysql_query(conn,sql.c_str) == NULL){
		perror("mysql_query!");
		ret = false;
	}
	cout<<"ret:"<<ret<<endl;
	return ret;
}
~SqlApi()
{

}
