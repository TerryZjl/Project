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
    printf("get listenfd\n");
    while(1)
    {
        printf(".........\n");
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int new_sock = accept(listenfd, (struct sockaddr*)&client, &len);
        printf("accept new sock \n");
        pthread_t id;
        pthread_create(&id, NULL,  handler_request, (void*)new_sock);
     
        printf("wwwwwwwwwwwwww\n");
        pthread_detach(id);
        printf("sssssssssssssss\n");
    }
    return 0;
}
