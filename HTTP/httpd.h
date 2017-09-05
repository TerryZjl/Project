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

//#define _STDOUT_
#define SIZE 1024


void* handler_request(void *arg);
int Getlistenfd(char *ip, int port);
#endif
