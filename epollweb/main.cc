#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<string.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<sys/sendfile.h>
#include<iostream>

using namespace std;

#include"processpool.h"

#define _STDOUT_ //控制print_log
//错误级别
#define SUCCESS 0
#define NOTICE  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

#define SIZE 1024

const char* status_line = "HTTP/1.0 200 OK\r\n";
const char* blank_line = "\r\n";
const char* type_line = "Content-Type: text/html;charset:utf-8\r\n";


class cgi_conn
{
public:
	void print_log(const char* str,int error,int lines)
	{
	#ifdef _STDOUT_ 
		const char *err[]= {"SUCCESS","NOTICE","WARNING","ERROR","FATAL"};//错误级别
		printf("lines:[%d] [%s] [%s]\n",lines, err[error%5],str);
	
		/*openlog("print_log",LOG_CONS|LOG_PID,LOG_USER);
		syslog(LOG_INFO,"lines:%d %s %s\n",lines,str,err[error%4],);
		closelog();*/
	#endif
	}
	
	
	void send_error(int sock,const char* path)
	{
		if(path == NULL)
		{
			print_log("error page path is NULL---",FATAL,__LINE__);
			return;
		}
		struct stat st;
		if(stat(path,&st) < 0)
		{
			print_log("error page not found---",FATAL,__LINE__);
			return;
		}
	
	    int fd = open(path,O_RDONLY);
		int ret = 0;
		if(ret = sendfile(sock,fd,0,st.st_size) < 0)
		{
			print_log("Error send error_page---",FATAL,__LINE__);
		}
	    close(fd);
	}
	
	void error_response(int sock, int error_code)
	{	
	        send(sock, status_line, strlen(status_line), 0);
	        send(sock, type_line, strlen(type_line), 0);
	        send(sock, blank_line, strlen(blank_line), 0);
		switch(error_code)
		{
		case 401:
			{
			send_error(sock,"error/401.html");
			break;
			}
		case 400:
			{
			send_error(sock,"error/400.html");
			break;
			}
		case 403:
			{
			send_error(sock,"error/403.html");
			break;
			}
		case 404:
			{
			send_error(sock,"error/404.html");
			break;
			}
		case 500:
			{
			send_error(sock,"error/500.html");
			break;
			}
		case 503:
			{
			send_error(sock,"error/503.html");
			break;
			}
		default:
			break;
		}
	
		sleep(60);
	}
	
	
	int exe_cgi(int sock,const char *path,\
	                   const char* method, const char* query_string, int len )
	{
	  
	    printf("enter exe_cgi 1 \n");
	    //环境变量
	    char method_env[SIZE];  //GET or POST
	    char query_string_env[SIZE]; //用户输入的参数
	    char content_len_env[SIZE];  //如果是POST，那么参数的长度是多长
	    int content_len = -1;
	
	    if(strcasecmp(method, "GET") == 0){//GET
	        //drop_header(sock);
			printf("GET method\n");
	    }else{//post
	        //先在请求报头里获得参数的长度，”Content-Length： “
			printf("POST method\n");
			int ret =-1;
	        do{
	            ret = get_line(len);
	            
				if(ret>0&&strncmp(line_buf, "Content-Length: ", 16) == 0){
					content_len = atoi(&line_buf[16]);//得到参数的长度
				}
	        }while(ret>0 && strcmp(line_buf, "\n")!=0);
			if(content_len < 0){
				//print_log("Content-Length is error---",FATAL,__LINE__);
				return 500;
			}
	    }
	    //发送状态行和空行
	    send(sock, status_line, strlen(status_line), 0);
	    send(sock, type_line, strlen(type_line), 0);
	    send(sock, blank_line, strlen(blank_line), 0);
	    //printf("send status_line!!\n");
	    //创建管道，用于把进程从浏览器读来的信息通过管道发送个子进程去处理
	    //子进程处理完毕数据并通过管道传会给父进程，父进程在传给浏览器
	    int input[2];
	    int output[2];
	    if(pipe(input)<0 || pipe(output)<0){
	        close(input[0]);
	        close(input[1]);
	        close(output[0]);
	        close(output[1]);
	        return 500;
	    }
	
	    // path指向的是个可执行的文件，则时候就要进行程序替换来运行可执行程序，但此时是在线程内，所以不可以直接接进行程序替换，一旦替换，就会将所有服务器的程序都替换了，所以要fork创建子进程来进行程序替换。
	    pid_t id = fork();
	    if(id< 0){
	        return 500;  
	    }else if(id == 0){//child
	        //这里管道的命名是站在子进程的角度
	        close(input[1]);
	        close(output[0]);
	        //把子进程的标准输入和输出重定向到两个管道上
	        //dup2(input[0],0);
	        //dup2(output[1], 1);
	        printf("dup2......\n"); 
	       //子进程要通过程序替换执行相应的程序，要用到上面从报头得到的参数和方法，如果是post方法，就还有参数长度，则需要把这些信息传递给子进程，我们知道进程不仅可以通过管道通信，子进程还可以继承父进程的环境变量，所以我们把这些信息变量导入到环境变量里,这样子进程就可以用这些信息了。那为什么不要用管道传输呢？因为这些信息的格式不一，不好控制格式，所以要用环境变量比较方便。
	        sprintf(method_env, "METHOD=%s", method);
	        putenv(method_env);
	            printf("GET query_string:%s\n", query_string);
	        printf("method_env: %s \n",method);
	 
			if(strcasecmp(method,"GET") == 0){
	            printf("GET query_string:%s\n", query_string);
	            sprintf(query_string_env,"QUERY_STRING=%s",query_string);
	            putenv(query_string_env);//将string导入到当前的环境中
	        }else{
	            printf("POST content_lenth:%d \n", content_len); 
	            sprintf(content_len_env,"CONTENT_LENGHT=%d",content_len);
	            printf("%s\n",content_len_env);
	            putenv(content_len_env);
	        }
			/*printf("method:%s\n",method_env);
			printf("query_string:%s\n",query_string_env);
			printf("content_lenth:%s\n",content_len_env);
	        */
			printf("PATH:%s\n",path);
	        dup2(input[0],0);
	        dup2(output[1], 1);
	        execl(path, path, NULL);//第一个参数是要执行谁，第二个参数在命令行上执。程序替换函数不用出错返回。
	        printf("execl\n"); 
	        exit(1);
	     }else{//father
	        close(input[0]);
	        close(output[1]);
	        char c;
	        if(strcasecmp(method, "POST") == 0){
	            int i = 0;
				int ret = get_line(len);       
				for(;i < content_len; i++)
	            {
					c = line_buf[i];
	                //recv(sock, &c , 1, 0);
	                write(input[1], &c, 1);
	            }
	        }
	        while(read(output[0], &c , 1)>0){
	            send(sock, &c, 1, 0);
	        }
	        waitpid(id, NULL, 0);
	        close(input[0]);
	        close(output[1]);
	    }
		return 200;
	}



	void init(int epollfd, int sockfd, const sockaddr_in& client_addr)
	{
		memset(line_buf, '\0', BUFFER_SIZE);
		m_epollfd = epollfd; //某子进程的epoll内核事件表socket
		m_sockfd = sockfd;         //客户socket
		memset(m_buf, '\0', BUFFER_SIZE);
		m_read_idx = 0;
		m_address = client_addr;
	}

	void process()
	{
		char method[20];
		char url[BUFFER_SIZE];
		//char path[BUFFER_SIZE] = "wwwroot/index.html";
		char path[BUFFER_SIZE];
	    memset(path, '\0', BUFFER_SIZE);
	    memset(method, '\0', 20);
	    memset(url, '\0', BUFFER_SIZE);
		int idx = 0;
		int ret = -1;
		int i = 0;
		int j = 0;
		int cgi = 0;
		int err_code = 200;
		char* query_string = NULL; //参数
 		while(true)
		{
			idx = m_read_idx;
			ret = recv(m_sockfd, m_buf+idx, BUFFER_SIZE-1-idx, 0);
			printf("%s\n\n",m_buf);
			if(ret < 0){
				if(errno != EAGAIN)
					removefd(m_epollfd,m_sockfd);
				break;
			}else if(ret == 0){
				removefd(m_epollfd,m_sockfd);
				break;
			}else{
				 /*
	  			 struct stat st;
	  			 if(stat(path, &st)< 0){
	  			     printf("not find path\n");
				 }
				 echo_www(m_sockfd, path,st.st_size);
				 */
				//获取请求行信息
				if(get_line(ret) <= 0){
				   perror("get_line");
					continue;
				}
				//获取请求方法
				while((i<(sizeof(method)-1))&& (j < sizeof(line_buf)) && (!isspace(line_buf[j]))){
	  			     method[i++] = line_buf[j++];
				 }
				 printf("method:%s\n",method); 			
	  			 while(j< sizeof(line_buf) && isspace(line_buf[j])){
	  			     j++;
	  			 }
	  			 //url
	  			 i = 0;
	  			 while(i < sizeof(url)-1 && j < sizeof(line_buf) && !isspace(line_buf[j])){
	  			  url[i] = line_buf[j];
	  			      i++;
	  			      j++;
	  			 }
	  			 printf("url:%s\n",url);

	  			 if(strcasecmp(method, "GET") == 0){
	  			     query_string = url;
	  			     while(*query_string){
	  			         if(*query_string == '?'){
	  			             *query_string = '\0';
	  			             query_string++;
	  			             cgi = 1;
	  			             break;
	  			         }
	  			         query_string++;
	  			     }
	  			 }
	  			 printf("query_string:%s\n",query_string);
	  			 sprintf(path, "wwwroot%s", url);
	  			 printf("path:%s\n",path);
	  			 if(path[strlen(path)-1]== '/'){
	  			     strcat(path, "index.html");
	  			 } 
	  			 printf("url path:%s\n",path);
				 struct stat st;
	  			 if(stat(path, &st)< 0){
	  			     printf("not find path\n");
	  			     err_code = 404;
	  			     goto end;        
	  			 }else{
	  			     if(S_ISDIR(st.st_mode)){//是目录
	  			         strcat(path, "/index.html");
	  			     }else if(st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH ){//exe
	  			         cgi = 1;
	  			   
	  			     printf("cgi path:%s\n",path);
	  			     }else{
	  			     }
	  			     if(cgi){

						 printf("exe_cgi path:%s\n", path);
	  			         printf("exe_cgi query_string:%s\n", query_string);
	  			         err_code = exe_cgi(m_sockfd, path, method, query_string,ret);           
	  			     }else{ 
						    
						 printf("echo_www path:%s\n",path);
	  			         //drop_header(m_sockfd);
	  			         echo_www(m_sockfd, path , st.st_size);
	  			     }
	  			 }
				end:   
				if(err_code != 200){
					printf("er_code = %d\n",err_code);
					error_response(m_sockfd, err_code);
				}
			    
				removefd(m_epollfd, m_sockfd);
				break;
			}
		}//while
	}
public:

	void echo_www(int sock, const char* path, int len)
	{
	    int fd = open(path,O_RDONLY);
	    if(fd < 0){
			printf("1\n");
			//print_log(strerror(errno),FATAL,__LINE__);
			return;
		}
		send(sock, status_line, strlen(status_line), 0);
	    send(sock, type_line, strlen(type_line), 0);
	    send(sock, blank_line, strlen(blank_line), 0);
		printf("%d\n",len);
		if(sendfile(sock, fd, NULL, len)<0){
			printf("2\n");
			//print_log(strerror(errno),FATAL,__LINE__);
			return;
		}
	    close(fd);
	}
	int get_line(int len)
	{
		if (len <= 0)
			return -1;
		memset(line_buf, '\0', BUFFER_SIZE);
		int i = m_read_idx;
		if (m_read_idx > len)
		{
			//这里推出说明缓冲区line_buf里的数据已经读完了，需要在recv一次。
			m_read_idx = 0;
			return -1;
		}
		int index = 0;
	    char c = '\0';
	    while((i < len) && c != '\n' ){
			c = m_buf[i++];
	        //ssize_t s = read(sock, &c, 1);
	        if( c == '\r'){ //\r->\n   \r\n->\n
	           //recv(sock, &c, 1, MSG_PEEK);//窥探下一个字符是什么，只看不取
	            c = m_buf[i+1];
				if(c=='\n'){
					++i; 
				}else{
	                c = '\n';
	            }
	        }
            line_buf[index++] = c;
	    }
		line_buf[index] = '\0';
		m_read_idx = (i+1);
		return index;
	}

private:
	static const int BUFFER_SIZE=1024;
	static int m_epollfd;		//某子进程的epoll内核事件表socket
	int m_sockfd;               //客户socket
	char m_buf[BUFFER_SIZE];    //从sock里把数据recv到这个buf里
	char line_buf[BUFFER_SIZE]; //读取请求数据一行的数据
	sockaddr_in m_address;
	int m_read_idx;             //标记缓冲区中已经读入的客户数据的最后一个字节的位置
};

int cgi_conn::m_epollfd = -1;

int Getlistenfd(int port, const char* ip)
{
	int listenfd = socket(PF_INET, SOCK_STREAM,0);
	assert(listenfd >= 0);
	
	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(port);

	ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
	assert(ret != -1);
	
	ret = listen(listenfd, 5);
	assert(ret != 1);

	return listenfd;
}

void Use()
{
	printf("Usage:\n");
	printf("	[./httpd] [ip] [port]\n");
}
int main(int argc, char* argv[])
{
	if(argc < 3){
		Use();
		return 1;
	}
	const char* ip = argv[1];
	int port = atoi(argv[2]);
	
	int listenfd = Getlistenfd(port, ip); //创建监听套接字

	processpool<cgi_conn>* pool = processpool<cgi_conn>::create(listenfd);
	if(pool){
		pool->run(); //启动进程池
		delete pool;
	}
	
	close(listenfd);
	return 0;
}
