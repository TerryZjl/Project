#include"sql_api.h"

void getdata(char* &name, char* &age, char* &sex, char* &phone) {
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
        char* str[10];
        int i = 0;
        while(*start)
        {
            if(*start == '='){
                start++;
                str[i++]=start;
                continue;
            }
            if(*start == '&'){
    	    *start = '\0';
    	}
            start++;
        }
        str[i] = NULL;
	
        name = str[0];
        age = str[1];
        sex = str[2];
        phone = str[3];		
}

int main()
{

        printf("<html><body>");
        printf("<meta http-equiv=Content-Type content=text/html; charset=utf-8>");
        printf("<style type=text/css>");
        printf("<!-- body,td,th {font-size: 15px;} body {background-color: #99FFFF; }-->");
        printf("</style></head>");
	
	SqlApi s("0",3306);
       
	char* name = NULL;
        char* age = NULL;
        char* sex = NULL;
        char* phone = NULL;	
	getdata(name, age,sex,phone);
		
	if(s.connect()==false){
		printf("<br>sql connect false!!<br>");
		sleep(3);
	}
	if(s.insert(name, age, sex, phone)==false){
		printf("<br>insert false!!!");
	}else{
		printf("<br><table border=1><tr><td>%s</td><td>%s</td><td>%s</td> <td>%s</td></td></tr></table><br>",name,age,sex,phone);
		printf("insert success!!!<br>");
	}
	
       	printf("<a href=/>确认返回</a>");
        sleep(2);
	printf("</body></html>");
	return 200;
}
