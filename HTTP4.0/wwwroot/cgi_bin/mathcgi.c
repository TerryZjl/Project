#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include <memory.h> 


//int day(int data1, int data2, int data3, int data4, int data5, int data6)
//{
//	time_t t1;
//	time_t t2;
//
//	struct tm OldTime;
//	struct tm NewTime;
//
//	memset(&OldTime, 0x0, sizeof(OldTime));
//	OldTime.tm_year = data1;// 2010 年表示为 2010 - 1900 = 110 
//	OldTime.tm_mon = data2;
//	OldTime.tm_mday = data3;
//
//	memset(&NewTime, 0x0, sizeof(NewTime));
//	NewTime.tm_year = data4;// 2011 年表示为 2011 - 1900 = 111 
//	NewTime.tm_mon = data5;
//	NewTime.tm_mday = data6;
//
//	t1 = mktime(&NewTime); //2011-12-6 
//	t2 = mktime(&OldTime); //2010-12-6 
//   
//	printf("%d\n",(t1 - t2) / (24 * 3600));
//	return ((t2 - t1) / (24 * 3600));
//}
int day(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end)
{
 int y2, m2, d2;
 int y1, m1, d1;
 
 m1 = (month_start + 9) % 12;
 y1 = year_start - m1/10;
 d1 = 365*y1 + y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + (day_start - 1);

 m2 = (month_end + 9) % 12;
 y2 = year_end - m2/10;
 d2 = 365*y2 + y2/4 - y2/100 + y2/400 + (m2*306 + 5)/10 + (day_end - 1);
 
 return (d2 - d1);
}

void math2(char* query_string)
{
    char* start = query_string;
    char* buf[10];
    int i = 0;
    while(*start)
    {
        if(*start == '='){
            start++;
            buf[i++]=start;
            continue;
        }
        if(*start == '&'){
	    *start = '\0';
	}
        start++;
    }
    buf[i] = NULL;
    int data1 = atoi(buf[0]);
    int data2 = atoi(buf[1]);
    int data3 = atoi(buf[2]);
    int data4 = atoi(buf[3]);
    int data5 = atoi(buf[4]);
    int data6 = atoi(buf[5]);
    int data7 = 0;
    
    printf("<html><body>");
    printf("<meta http-equiv=Content-Type content=text/html; charset=utf-8>");
    data7 = day(data1,data2,data3,data4,data5,data6);
   
    printf("%d年%d月%d日   到   %d年%d月%d天=   %d  天\n",data1, data2, data3, data4,data5,data6,data7);

    printf("<style type=text/css>");
    printf("<!-- body,td,th {font-size: 15px;} body {background-color: #99FFFF; }-->");
    printf("</style></head>");
    printf("<br><br><br><a href=/>返回首页</a>");
    printf("</body></html>");
}

int main()
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
    math2(query_string);
    return 200;
}
