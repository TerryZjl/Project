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
	if(NULL == mysql_real_connect(conn,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,NULL, 0)){	
		const char* s = mysql_error(conn);
		ret = false;
		printf("error:%s\n",s);
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
	}
	return ret;
}
bool SqlApi::sql_select(string &sql)
{
        printf("<html><body>");
        printf("<meta http-equiv=Content-Type content=text/html; charset=utf-8>");
        printf("<style type=text/css>");
        printf("<!-- body,td,th {font-size: 15px;} body {background-color: #99FFFF; }-->");
        printf("</style></head>");

	bool ret = true;
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
	
	
	printf("<table border=8>");
	printf("<tr><td>查询结果</td></tr><tr>");
	while(field = mysql_fetch_field(result)){//获取列名
		printf("<td>%s</td>",field->name);
	}
	printf("</tr>");

	num = mysql_num_fields(result);  //获取列数
	while(row = mysql_fetch_row(result)){//获得一行的内容
		printf("<tr>");
		for(i = 0; i < num; i++){
		printf("<td>%s</td>",row[i]?row[i]:"NULL");
		}	
		printf("</tr>");
	}	
	printf("</table>");
        printf("<br><br><br><a href=/>返回首页</a>");
        printf("</body></html>");
	
	mysql_free_result(result);//mysql_store_result(conn)读取查询结果并malloc一片内存空间存放查询来的数据，所以在此释放内存。
	return ret;	
}

bool SqlApi::sql_select_part(char *&str)
{
	bool ret = true;
	string sql = "select * from information where name='";	
	sql+=str;
	sql+="'";
	ret = SqlApi::sql_select(sql);
	return ret;
}
bool SqlApi::sql_select_all()
{
	bool ret = true;
	string sql = "select * from information";	

	ret = SqlApi::sql_select(sql);
	return ret;	
}

SqlApi::~SqlApi()
{
	mysql_close(conn);
}
