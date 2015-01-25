//回显服务器服务端代码
//简单起见服务器监听地址仅设为本地环路（127.0.0.1）
//服务端口号为56789
#include <iostream>
#include <vector>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <netinet/in.h>
#include <bits/socket.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

const string serveraddr = "127.0.0.1";
const int serverport = 56784;

//全局量Vector用于存储客户端connfd
vector<int> ClientConnfd;
int listenfd;

void DoEcho(fd_set* readfd);

void HandleInt(int signal)
{
	cout << "servet is closed" << endl;
	close(listenfd);
	for(int connfd : ClientConnfd)
		close(connfd);
	exit(0);
}

//初始化并绑定监听socket并返回
int InitListenSock(const string& ip,int port)
{
	//先创建socket
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1)
	{
		cout << "创建监听socket失败" << endl;
		exit(1);
	}
	//设定服务器地址结构体
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip.c_str(),&saddr.sin_addr);
	saddr.sin_port = htons(port);
	//绑定
	if(bind(listenfd,(struct sockaddr*)&saddr,sizeof(saddr)) == -1)
	{
		cout << "绑定失败" << endl;
		exit(1);
	}
	//返回绑定后的监听socket
	return listenfd;
}

//用于使用select处理IO复用
//参数必定是一个已开始监听的listenfd
void DoSelect(int listenfd)
{
	//客户端连接socket描述符，将存储在vector中
	int connfd;
	//记录最大描述符值
	int maxfd;
	//记录select返回的可用描述符个数
	int ready;
	//客户端地址结构体以及其长度
	sockaddr_in caddr;
	socklen_t caddrlen = sizeof(caddr);
	//文件描述符fd_set结构（只用于判断可读性）
	fd_set readfd;

	//首先将监听socket描述符添加进fd_set结构体
	FD_SET(listenfd,&readfd);
	//更新最大描述符值
	maxfd = listenfd;

	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	//开始循环
	while(true)
	{
		// for(int connfd : ClientConnfd)
		// 	FD_SET(connfd,&readfd);
		// FD_SET(listenfd,&readfd);
		//调用select
		ready = select(maxfd + 1,&readfd,NULL,NULL,&timeout);
		//select出错
		if(ready == -1)
		{
			cout << "select出错" << endl;
			exit(1);
		}
		else if(ready == 0)
		{
			cout << "time out " << endl;
			exit(1);
		}
		//检查监听描述符是否就绪
		if(FD_ISSET(listenfd,&readfd))
		{
			//如果就绪说明有新的连接可以接受
			if((connfd = accept(listenfd,(struct sockaddr*)&caddr,&caddrlen)) == -1)
			{
				if(errno == EINTR)
					continue;
				else
				{
					cout << "accept出错" << endl;
					exit(1);
				}
			}
			//输出新连接信息到终端
			cout << "客户端：" << inet_ntoa(caddr.sin_addr) << "已连接" << endl;
			//将新连接的客户端连接描述符添加进vector
			ClientConnfd.push_back(connfd);
			//同时将该描述符添加进select集合中
			FD_SET(connfd,&readfd);
			//更新最大描述符值
			maxfd = (maxfd < connfd ? connfd : maxfd);
			//判断是否只有监听描述符就绪（即没有已连接的客户端发送消息，只有新连接请求）
			if(ready <= 1)
			{
				cout << "continue" << endl;
				continue;			
			}
		}
		//循环处理已连接客户端中的回显功能
		DoEcho(&readfd);
	}
}

//用于处理已连接的客户端中每一个客户的回显功能
void DoEcho(fd_set* readfd)
{
	for(vector<int>::iterator it = ClientConnfd.begin();
		it != ClientConnfd.end();)
	{
		//如果该客户端连接描述符已就绪即可读写数据
		if(FD_ISSET(*it,readfd))
		{
			char buf[1024];
			int n = recv(*it,buf,sizeof(buf),0);
			if(n == 0)
			{
				close(*it);
				FD_CLR(*it,readfd);
				//删除元素
				it = ClientConnfd.erase(it);
				continue;
			}
			else
			{
				++it;
				//将数据返回
				send(*it,buf,sizeof(buf),0);
			}
		}
	}
}

int main()
{
	signal(SIGINT,HandleInt);
	listenfd = InitListenSock(serveraddr,serverport);
	listen(listenfd,5);
	cout << "正在监听..." << endl;
	DoSelect(listenfd);
	return 0;
}