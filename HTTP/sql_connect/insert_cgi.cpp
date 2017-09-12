#include"sql_api.h"



int main()
{
	SqlApi s("127.0.0.1",3306);
	if(s.connect()==false){
		return 1;
	}
	if(s.insert("zhao", "21","boy", "123")==false){
		return 2;
	}
	return 0;
}
