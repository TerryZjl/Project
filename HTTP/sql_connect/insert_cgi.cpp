#include"sql_api.h"


int main()
{
	SqlApi s;
	string str;
	str = "Terry,21,man,18729576689";
	if(s.connect()==false){
		return 1;
	}
	if(s.insert(str)==false){
		return 2;
	}
	cout<<" insert"<<endl;
	return 0;
}
