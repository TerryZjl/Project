#include"sql_api.h"

SqlApi::SqlApi(const string &_h,\
       const int &_port,\
       const string &_u,\
       const string &_p,\
       const string &_db)
{	
	conn = mysql_init(NULL);
	if(conn == NULL){
		cout<<"mysql_init error!!!"<<endl;
	}
	host = _h;
	user = _u;
	passwd = _p;
	db = _db;
	port = _port;	
	cout<<"init ok"<<host<<user<<db<<port<<endl;
}

bool SqlApi::connect()
{	
	bool ret = true;
	if(mysql_real_connect(conn,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,NULL, 0)){
		printf("sql connect success!!\n");
	}
	else{	
		const char* s = mysql_error(conn);
		ret = false;
		cout<<s<<endl;
		printf("sql connect false!!\n");
	}
	return ret;
}
bool  SqlApi::insert(const string &_name,\
		     const string &_age,\
		     const string &_sex,\
		     const string &_phone)
{	
	bool ret = true;
	string sql = "INSERT INTO information (name,age,sex,phone) VALUES ('";
	sql += _name;	
	sql += "','";
	sql += _age;
	sql += "','";
	sql += _sex;
	sql += "','";
	sql += _phone;
	sql += "')";
	cout<<sql<<endl;
	if(mysql_query(conn,sql.c_str()) != 0){
		ret = false;
		cout<<"insert fasle"<<endl;
	}
	cout<<"insert success!!"<<endl;
	cout<<"ret:"<<ret<<endl;
	return ret;
}
