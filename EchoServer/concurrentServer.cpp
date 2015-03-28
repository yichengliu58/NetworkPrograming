//concurrent echo server
//from UNP p98
#include <iostream>
#include <utility>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>

#include <netinet/in.h>
#include <bits/socket.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

//initialize and bind the socket and return it
int InitListenSock(const string& ip,int port)
{
	//create socket
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1)
	{
		cout << "创建监听socket失败" << endl;
		exit(1);
	}
	//set address structure
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip.c_str(),&saddr.sin_addr);
	saddr.sin_port = htons(port);
	//binding
	if(bind(listenfd,(struct sockaddr*)&saddr,sizeof(saddr)) == -1)
	{
		cout << "绑定失败" << endl;
		exit(1);
	}
	return listenfd;
}

//do echo
void DoEcho(int connfd)
{
	int n;
	char buf[1024];
	while(n = read(connfd,buf,sizeof(buf)) > 0)
	{
		write(connfd,buf,sizeof(buf));
	}

}
int main(int argc,char* argv[])
{
	//get cmd arguments;
	string addr(argv[1]);
	int port = stoi(argv[2]);

	//two socket fds
	int listenfd,connfd;
	//child process fd
	pid_t childPid;
	//client structure length
	socklen_t clientLen;
	//client structure
	sockaddr_in clientAddr;
	//binding
	listenfd = InitListenSock(addr,port);
	//begin listen
	listen(listenfd,5);
	//begin loop
	for(;;)
	{
		clientLen = sizeof(clientAddr);
		//will block at accept
		connfd = accept(listenfd,(sockaddr*)&clientAddr,&clientLen);
		//fork new process
		if(childPid = fork() == 0)
		{
			//in child process
			//first close socked opened by parent
			close(listenfd);
			//do echo
			DoEcho(connfd);
			//exit process automatically close connfd
			exit(0);
		}
		//else in parent process
		close(connfd);
	}
}