#include"sql_api.h"


int main()
{	
	SqlApi q("127.0.0.1",3306);
	q.connect();
	q.sql_select();
	return 0;
}
