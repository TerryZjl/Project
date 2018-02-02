#include"sql_api.h"

char* getdata()
{
	int l = 0;
        char* start = NULL;
        char* method = getenv("METHOD");
        char* query_string = NULL;
        int content_len = -1;
        char buf[1024];
        if(method){
            if(strcasecmp(method, "GET") == 0){
                query_string = getenv("QUERY_STRING");
            }else{
                content_len = atoi(getenv("CONTENT_LENGHT"));
                for(;l< content_len; l++)
                {
                    read(0, &buf[l], 1);//从管道里读取父进程传来的POST参数内容
                }
                buf[l] = '\0';
                query_string = buf;
            }
        }
        start = query_string;
        char* str = NULL;	
        int i = 0;
        while(*start)
        {
            if(*start == '='){
                start++;
                str = start;
                continue;
            }
            if(*start == '&'){
    	    *start = '\0';
    	}
            start++;
        }
	return str;
}
int main()
{	
	SqlApi q("0",3306);
	if(q.connect()==false){
		printf("<br>sql connect false!!<br>");
		sleep(3);
	}
	char *str = NULL;
	str = getdata();
	if(str == NULL){
		q.sql_select_all();
	}else if(str != NULL){
		q.sql_select_part(str);
	}else{
		printf("!!!!!!!!!!!!");
	}
	return 200;
}
