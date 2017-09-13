#include<iostream>
#include<string.h>
#include<strings.h>
#include<stdio.h>
using namespace std;


int main()
{
        cout<<"enter insert"<<endl;
	int l = 0;
        char* start = NULL;
        string method = "GET";
        string query_string = "name=zhao&age=21&sex=man&data5=5";
        int content_len = -1;
        char buf[1024];
        if(!method.empty()){
            if(true){
                query_string = "name=zhao&age=21&sex=man&data5=5";
            }else{
		cout<<"POST"<<endl;
                content_len = 0;
            }
        }
	printf("get query_string\n");
        start = query_string.c_str();
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
        char* name = str[0];
        char* age = str[1];
        char* sex = str[2];
        char* phone = str[3];	
	printf("%s  %s %s  %s\n",name,age,sex,phone);
	printf("insert success!!\n");
	return 0;
}
