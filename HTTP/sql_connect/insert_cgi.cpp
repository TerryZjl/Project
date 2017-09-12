#include"sql_api.h"


int main()
{
	SqlApi s("127.0.0.1",3306);
	if(s.connect()==false){
		return 1;
	}
	if(s.insert("zhangsan","25","man","123456")==false){
		return 2;
	}
	return 0;
}
