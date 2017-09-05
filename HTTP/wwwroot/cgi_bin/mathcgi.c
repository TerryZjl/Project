#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>

void math(char* query_string)
{
     printf("hello cgi!!!!\n");
     char* buf[3];
     int i = 0;
     int j = 0;
     char* start = query_string;
     printf("start : %s\n",start);
     while(*start!='\0')
     {
     	if(*start == '='){
            start++;
            buf[i++] = start; 
            continue;      
     	}
        if(*start == '&')
        {
            *start = '\0';
        }
        start++;
     }
     buf[i] = NULL;
     int data1 = atoi(buf[0]);
     int data2 = atoi(buf[1]);
     printf("%d + %d = %d\n", data1, data2, data1+data2);
     printf("%d - %d = %d\n", data1, data2, data1-data2);
     printf("%d * %d = %d\n", data1, data2, data1*data2);
     //printf("data1 / data2 = %d\n", data1==0 ? 0 : (data1/data2));
}

int main()
{
    char* method = getenv("METHOD");
    char* query_string = NULL;
    int content_len = -1;
    char buf[1024];
    if(method){
        if(strcasecmp(method, "GET") == 0){
            query_string = getenv("QUERY_STRING");
        }else{
            content_len = atoi(getenv("CONTENT_LENGHT"));
            int i = 0;
            for(;i< content_len; i++)
            {
                read(0, &buf[i], 1);//从管道里读取父进程传来的POST参数内容
            }
            buf[i] = '\0';
            query_string = buf;
        }
    }
    math(query_string);
    return 0;
}
