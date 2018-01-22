#ifndef _HTTP_H_
#define _HETTP_H_

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<syslog.h>
#include<ctype.h>
#include<sys/stat.h>
#include<errno.h>

#define _STDOUT_ //控制print_log
//错误级别
#define SUCCESS 0
#define NOTICE  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

#define SIZE 1024

void print_log(const char* str,int error,int lines);
void send_error(int sock,const char* path);
void error_response(int sock,int error_code);
void* handler_request(void *arg);
int Getlistenfd(char *ip, int port);
#endif
