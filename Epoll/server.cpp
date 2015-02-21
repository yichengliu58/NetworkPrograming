#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <bits/socket.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <list>
#include <cmath>
#include <memory>
#include <memory.h>
#include <assert.h>

using namespace std;

struct ClientInfo
{
	struct sockaddr_in address;
	int connfd;
};

list<ClientInfo> Clients;

int main(int argc,char* argv[])
{
	string ip = argv[1];
	int port = atoi(argv[2]);
	//设定服务器地址结构体
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip.c_str(),&saddr.sin_addr);
	saddr.sin_port = htons(port);

	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	assert(listenfd > 0);

	int ret = bind(listenfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(ret == 0);

	int eventTable = epoll_create(5);
	assert(eventTable > 0);

	struct epoll_event listenevent;
	listenevent.events = EPOLLIN;
	listenevent.data.fd = listenfd;

	epoll_ctl(eventTable,EPOLL_CTL_ADD,listenfd,&listenevent);

	struct epoll_event events[128];

	ret = listen(listenfd,5);
	assert(ret == 0);


	while(true)
	{
		//sleep(2);
		ret = epoll_wait(eventTable,events,128,-1);
		//cout << "finish waiting " << ret << endl;
		char buf[1024];
		for(int i = 0;i < ret;i++)
		{
			int readyfd = events[i].data.fd;
			//cout << "readyfd num : " << readyfd << " i = " << i << endl;
			if(readyfd == listenfd)
			{
				ClientInfo client;
				socklen_t size = sizeof(client.address);
				client.connfd = accept(listenfd,(struct sockaddr*)&(client.address),&size);
				//cout << "accept new client: " << client.connfd << endl;
				Clients.push_back(client);
				assert(client.connfd != -1);
				
				struct epoll_event clientevent;
				clientevent.events = EPOLLIN;
				clientevent.data.fd = client.connfd;
				epoll_ctl(eventTable,EPOLL_CTL_ADD,client.connfd,&clientevent);
			}
			/*if(events[i].events & EPOLLIN)
				cout << "epoll in " << events[i].data.fd << endl;
			if(events[i].events & EPOLLRDNORM)
				cout << "epoll rdnorm " << events[i].data.fd << endl;
			if(events[i].events & EPOLLPRI)
				cout << "pri " << events[i].data.fd << endl;
			if(events[i].events & EPOLLOUT)
				cout << "out " << events[i].data.fd << endl;
			if(events[i].events & EPOLLWRNORM)
				cout << "wrnorm " << events[i].data.fd << endl;
			if(events[i].events & EPOLLRDHUP)
				cout << "hang up " << events[i].data.fd << endl;
			if(events[i].events & EPOLLERR)
				cout << "err " << events[i].data.fd << endl;
			if(events[i].events & EPOLLHUP)
				cout << "hup" << events[i].data.fd << endl;*/
			else
			{
				if(events[i].events & EPOLLIN)
				{
					//cout << "data in " << endl;
					memset(buf,'\0',sizeof(buf));
					int res = recv(readyfd,buf,1024,0);
					if(res <= 0)
					{
						close(readyfd);
						for(list<ClientInfo>::iterator it = Clients.begin();it != Clients.end();)
						{
							if(it->connfd == readyfd)
								it = Clients.erase(it);
							else
								++it;
						}
					}
					else
						cout << Clients << readyfd << " : " << buf << endl; 
				}
				else
					cout << "something strange happened " << endl;
			}
		}
	}
}