#include"httpd.h"

void Usage(char *argv)
{
    printf("Usage\n\t%s [ip] [port]\n", argv);
}

int main(int argc, char* argv[])
{ 
	if(argc<3){
        Usage(argv[0]);
        return 1;
    }
    int listenfd = Getlistenfd(argv[1],atoi(argv[2]));
    //daemon(0,0);
	//精灵话需要更改工作目录，然后在后台工作，精灵进程不允许向前台打印消息，因此需要把打印的信息都打印到日志里。
	
	printf("get listenfd\n");
    while(1)
    {
        printf("accept wait ...\n");
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int new_sock = accept(listenfd, (struct sockaddr*)&client, &len);
		if(new_sock < 0){
			print_log(strerror(errno),NOTICE,__LINE__);
			continue;
		}
		printf("accept new sock= [%s:%d]ok\n", inet_ntoa(client.sin_addr),ntohs(client.sin_port));
        pthread_t id;
        int ret = pthread_create(&id, NULL,  handler_request, (void*)new_sock);
		if ( ret != 0 ){
			print_log(strerror(errno),NOTICE,__LINE__);
			close(new_sock);
			continue;
		}else{
			printf("pthread_create ok\n");
			pthread_detach(id);
			printf("pthread_detach ok\n");
		}
    }
	close(listenfd);
    return 0;
}
