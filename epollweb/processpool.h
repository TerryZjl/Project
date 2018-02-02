#ifndef _PROCESSPOOL_H_
#define _PROCESSPOOL_H_

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
#include<iostream>
using namespace std;

//描述子进程的类
class process
{
public:
	process()
		:m_pid(-1)
	{}

public:
	pid_t m_pid;     //目标子进程的pid
	int m_pipefd[2]; //子进程和父进程通信的管道
};

//进程池的类，模板T是任务处理的类
template<class T>
class processpool
{
private:
	/*将进程池的构造设置成私有的，因此只能用下面的静态create函数创建对象*/
	processpool(int listenfd, int process_number = 8);
public:
	static processpool<T>* create(int listenfd, int process_number = 8)
	{
		if(!m_instance){
			m_instance = new processpool<T>(listenfd, process_number);
		}
		return m_instance;
	}
	~processpool()
	{
		delete [] m_sub_process;
	}
	
	void run(); //启动进程池
private:
	void setup_sig_pipe();
	void run_parent();
	void run_child();

private:
	static const int MAX_PROCESS_NUMBER = 16;  //进程池中最大允许的进程数
	static const int USER_PER_PROCESS = 65536; //每个子进程最多能处理的客户数量
	static const int MAX_EVENT_NUMBER = 10000; //epoll最多能注册的事件数目
	static processpool<T>* m_instance;         //进程池静态实例,静态成员只能被静态函数访问

	int m_listenfd;					   //监听套接字
	int m_process_number;		       //进程池中的子进程总数
	int m_idx;						   //子进程在进程池中的序号，从0开始
	int m_stop;				           //子进程通过此参数决定是否停止允
	process* m_sub_process;			   //保存所有子进程的描述信息
	int m_epollfd;			           //每个进程都有一个epoll内核事件表
};

template<class T>
processpool<T>* processpool<T>::m_instance = NULL; //进程池实例初始化

static int sig_pipefd[2];//用于处理信号的管道，以实现统一事件源，称信号管道


//将文件表述符设置成非阻塞
static int setnonblocking(int fd)
{
	//获取和设置文件描述符状态
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

//向事件表内注册事件
static void addfd(int epollfd, int fd)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN|EPOLLET; //设置关心的事件，和边缘触发模式
	epoll_ctl(epollfd, EPOLL_CTL_ADD,fd, &event);
	setnonblocking(fd);
}

static void sig_handler(int sig)
{
	int save_errno = errno;
	int msg = sig;
	send(sig_pipefd[1],(char*)&msg, 1,0);
	errno = save_errno;
}

static void addsig(int sig, void(handler)(int), bool restart = true)
{
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = handler;
	if(restart){
		sa.sa_flags |= SA_RESTART;//重启被这个信号中断的系统调用，除了SIGALRM
	}
	sigfillset(&sa.sa_mask); //将sa_mask置满，包含系统支持的所有信号
	assert(sigaction(sig, &sa, NULL) != -1);
}

static void removefd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL,fd,0);
	close(fd);
}

//统一事件源
template<class T>
void processpool<T>::setup_sig_pipe()
{
	m_epollfd = epoll_create(5); //创建红黑树	
	assert(m_epollfd != -1);
	
	int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sig_pipefd);//创建信号管道
	assert(ret != -1);

	setnonblocking(sig_pipefd[1]);
	addfd(m_epollfd, sig_pipefd[0]);
	
	//设置信号处理信号
	addsig(SIGCHLD, sig_handler);
	addsig(SIGTERM, sig_handler); //kill
	addsig(SIGINT, sig_handler);  //ctrl+c
	addsig(SIGPIPE, SIG_IGN);     //读段关闭，写段发信号
}

//进程池的构造函数，listenfd为监听套接子, process_number为创建子进程数量
template<class T>
processpool<T>::processpool(int listenfd, int process_number)
	 :m_listenfd(listenfd)
	 ,m_process_number(process_number)
	 ,m_idx(-1)
	 ,m_stop(false)
{
	assert((process_number>0) && (process_number<=MAX_PROCESS_NUMBER));
	
	m_sub_process = new process[process_number];
	assert(m_sub_process!=NULL);

	//创建process_number个子进程,并建立他们和父进程之间的管道
	for(int i = 0 ;i<process_number ;++i){
		int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, m_sub_process[i].m_pipefd);
		assert(ret == 0);

		m_sub_process[i].m_pid = fork();
		assert(m_sub_process[i].m_pid >=  0);

		if(m_sub_process[i].m_pid == 0){ //子进程
			close(m_sub_process[i].m_pipefd[0]);
			m_idx = i;
			break; //子进程创建完毕，跳出循环
		}else{ //父进程
			close(m_sub_process[i].m_pipefd[1]);
			continue; //父进程继续创建管道和子进程
		}
	}
}

//父进程中的m_idx==-1,子进程中的m_idx >= 0,所以通过下面的m_idx判断执行谁的代码
template<class T>
void processpool<T>::run()
{
	if (m_idx != -1){
		run_child();
		return;
	}
	run_parent();
}

template<class T>
void processpool<T>::run_child()
{	
	setup_sig_pipe();

	/*每个进程通过自己的m_idx找到与父进程通信的管道*/
	int pipefd = m_sub_process[m_idx].m_pipefd[1]; 

	/* 从管道里接收父进程的通知，表明有客户来链接，然后指定子进程进行accept和处理
	 * 因此子进程需要监听管道文件，将管道文件的读端加入到内核事件表里*/
	addfd(m_epollfd, pipefd);

	epoll_event events[MAX_EVENT_NUMBER];
	T* users = new T[USER_PER_PROCESS];
	assert(users);
	int number = 0;
	int ret = -1;

	while(!m_stop)
	{
		number = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
		if((number < 0)&(errno != EINTR)) //由系统中断引起EINTR
		{
			printf("child pid=%d epoll failure\n",m_sub_process[m_idx].m_pid);
			break;
		}
		for(int i = 0;i < number; ++i)
		{
			int sockfd = events[i].data.fd;
			if((sockfd == pipefd) && (events[i].events & EPOLLIN)){
				//如果pipefd就绪，说明有客户来链接，下面接收链接，并处理客户后续的请求
				int client = 0;
				ret = recv(sockfd, (char*)&client, sizeof(client),0);
				if((ret < 0) && (errno != EAGAIN) ||  ret == 0){
					continue;
				}else{
					struct sockaddr_in client_address;//输出型参数，代表客户的信息
					socklen_t client_addrlength = sizeof(client_address);
					int connfd = accept(m_listenfd, (struct sockaddr*)&client_address, &client_addrlength);
					if (connfd < 0){
						printf("accept failure! errno is %d\n",errno);
						continue;
					}
					addfd(m_epollfd, connfd); //将链接的socket注册到当前子进程的事件表里
					//模板T的init，用于初始化一个客户的链接，我们直接使用connfd作为索引下标
					users[connfd].init(m_epollfd, connfd, client_address);
				}
			}else if((sockfd == sig_pipefd[0])&&(events[i].events & EPOLLIN)){
				//说明子进程收到信号了，则进行相应的信号处理
				int sig;
				char signals[1024]; //用于存放接收,用char表示一个信号，方便下面用整形来遍历的处理信号
				ret = recv(sig_pipefd[0],signals, sizeof(signals),0);
				if(ret < 0){
					continue;
				}else{
					for(int i = 0; i < ret;++i)
					{
						switch(signals[i])
						{
							case SIGCHLD:
							{
								pid_t pid;
								int stat;
								while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
								{
									printf("child's child pid=%d join\n",pid);
									continue;
								}
								break;
							}
							case SIGTERM:
							case SIGINT:
							{
								//子进程若收到上面这两个信号，则终止自己
								m_stop = true;
								break;
							}
							default:
							{
								break;
							}
						}//switch
					}//for
				}//fi
			}else if( events[i].events & EPOLLIN){
				//这里必然是客户的请求到来，则通过模板T里的process方法来处理客户的后续请求
				users[sockfd].process();
				printf("一个客户请求执行完毕\n");
			}else{
				continue;
			}//fi
		} //for
	}//while(!m_stop)
	delete [] users;
	users = NULL;
	close(pipefd);
	close(m_epollfd);
}

template<class T>
void processpool<T>::run_parent()
{
	setup_sig_pipe();

	//父进程监听m_listenfd
	addfd(m_epollfd, m_listenfd);
	
	epoll_event events[MAX_EVENT_NUMBER];
	int sub_process_counter = 0;
	int new_conn = 1;
	int number = 0;
	int ret = -1;

	while(!m_stop)
	{
		number = epoll_wait(m_epollfd, events,MAX_EVENT_NUMBER, -1); //阻塞等待有客户链接服务器
		if ((number < 0) && (errno != EINTR)){
			printf("parent epoll failure\n");
			break;
		}
		
		for(int i = 0; i< number;i++)
		{
			int sockfd = events[i].data.fd;
			if(sockfd == m_listenfd){
				//如果有新链接到来时，就采用论转的方法为其分配一个子进程
				int j = sub_process_counter;
				do
				{
					if(m_sub_process[j].m_pid != -1){
						break; //轮循的选出一个子进程
					}
					j = (j+1)% m_process_number;
				}while(j != sub_process_counter);

				//若没有选到子进程，则说明子进程全终止了，则将m_stop置为true，然后父进程退出
				if(m_sub_process[j].m_pid == -1)
				{
					m_stop = true;
					printf("father say godbye\n");
					break;
				}

				sub_process_counter = (j+1)% m_process_number; //轮循
		
				//通过j下标处的管道给子进程发送new_conn,让子进程进行连接客户和处理任务。
				send(m_sub_process[j].m_pipefd[0], (char*)&new_conn, sizeof(new_conn), 0);
				printf("send request to child %d\n", j);
			
			}else if((sockfd == sig_pipefd[0]) &&(events[i].events & EPOLLIN)){ //下面处理父进程的信号,监听有信号来了
				int sig;
				char signals[1024];
				ret = recv(sig_pipefd[0], signals, sizeof(signals),0);
				if(ret <= 0){
					continue;
				}else{
					for(int i = 0; i < ret; ++i)
					{
						switch(signals[i])
						{
							case SIGCHLD:
							{
								pid_t pid;
								int stat;
								while((pid = waitpid(-1,&stat, WNOHANG)) > 0 )
								{	
									for(int i = 0; i< m_process_number;++i)
									{
										if( m_sub_process[i].m_pid == pid)
										{
											printf("child %d join\n", i);
											close(m_sub_process[i].m_pipefd[0]);
											m_sub_process[i].m_pid = -1;
										}
									}
								}
								//先将m_stop置成true，若所有子进程都退出，那么父进程也就退出
								m_stop = true;
								for(int i= 0 ; i < m_process_number; ++i)
								{
									if(m_sub_process[i].m_pid != -1)
										m_stop = false;
								}
								break;
							}
							case SIGTERM:
							case SIGINT:
							{
								//如果父进程收到终止信号，那么父进程杀死所有的子进程，然后父进程在上面waitpid后再终止。
								printf("kill all the child\n");
								for(int i = 0 ; i < m_process_number; ++i)
								{
									int pid = m_sub_process[i].m_pid;
									if( pid != -1){
										kill(pid, SIGTERM); //子进程收到这个信号会将自己的m_stop设置成true，然后子进程退出
									}
								}
								break;
							}
							default:
							{
								break;
							}
						}//switch
					}//for
				}
			}
			else
			{
				continue;
			}
		}//for

	}//while(!m_stop)
	close(m_epollfd);
}
#endif //_PROCESSPOOL_H_
