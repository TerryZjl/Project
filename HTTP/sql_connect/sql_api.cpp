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
	if(mysql_query(conn,sql.c_str()) != 0){
		ret = false;
		cout<<"insert fasle"<<" ";
	}else{
	cout<<"insert success!!"<<" ";
	}
	cout<<"ret:"<<ret<<endl;
	return ret;
}

bool SqlApi::sql_select()
{
	bool ret = true;
	string sql = "select * from information";	
	if(mysql_query(conn,sql.c_str()) != 0){
		ret = false;
		cout<<"select false!!"<<endl;
	}
	MYSQL_RES *result = NULL;
	MYSQL_ROW row = NULL;
	MYSQL_FIELD *field = NULL;
	int num = 0;	
	int i = 0;
	result = mysql_store_result(conn);//获取查询数据
	cout<<endl;
	while(field = mysql_fetch_field(result)){//获取列名
		printf("%-15s",field->name);
	}
	cout<<endl;
	
	num = mysql_num_fields(result);  //获取列数
	while(row = mysql_fetch_row(result)){//获得一行的内容
		for(i = 0; i < num; i++){
		printf("%-15s",row[i]?row[i]:"NULL");
		}	
		cout<<endl;
	}	
	cout<<endl;
	mysql_free_result(result);//mysql_store_result(conn)读取查询结果并malloc一片内存空间存放查询来的数据，所以在此释放内存。
}

SqlApi::~SqlApi()
{
	mysql_close(conn);
}
