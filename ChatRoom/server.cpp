#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <memory.h>

#include <iostream>
#include <string>
#include <cmath>
using namespace std;
//记录客户数据
struct ClientData
{
	//表示客户端地址
	sockaddr_in addr;
	//表示待写到客户端的数据
	string writedata;
	//从客户端读入的数据
	char readdata[64];
};

//用于设置socket为非阻塞
int SetNonblocking(int socketfd)
{
	int old = fcntl(socketfd,F_GETFL);
	int ne = old | O_NONBLOCK;
	fcntl(socketfd,F_SETFL,ne);
	return old;
}

int main(int argc, char const *argv[])
{
	if(argc <= 2)
	{
		cout << "Usage: " << argv[0] << "ip port" << endl;
		exit(1);
	}

	const string ip = argv[1];
	int port = std::atoi(argv[2]);

	//执行初始化的一些操作
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip.c_str(),&serveraddr.sin_addr);
	serveraddr.sin_port = htons(port);

	int listenfd = socket(PF_INET,SOCK_STREAM,0);
	assert(listenfd >= 0);

	int ret = bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	assert(ret != -1);

	ret = listen(listenfd,5);
	assert(ret != -1);

	//创建client数组，用来表示已连接的客户端对象
	//用连接socket的值作为索引，用来方便的将两者关联起来
	ClientData* client = new ClientData[65535];
	//创建pollfd结构体数组，用来表示每一个连接描述符的事件
	//假设最多只接受五个连接
	pollfd fds[5];
	//客户端的计数器
	//表示pollfd数组中被监听的描述符数量
	int clientcount;
	//初始化pollfd数组
	for(int i = 0;i < 5;i++)
	{
		fds[i].fd = -1;
		fds[i].events = 0;
	}
	//先设置监听描述符的事件
	fds[0].fd = listenfd;
	fds[0].events = POLLIN | POLLERR;
	fds[0].revents = 0;

	//开始轮询
	while(1)
	{
		ret = poll(fds,clientcount + 1,-1);
		if(ret < 0)
		{
			cout << "poll失败" << endl;
			break;
		}
		//分别对pollfd数组中每一个就绪的描述符的事件做处理
		for(int i = 0;i < clientcount + 1;i++)
		{
			//判断监听描述符是否可读（新连接）
			//有则接受连接，并且将新的客户信息写入client数组
			if(fds[i].fd == listenfd && (fds[i].revents & POLLIN))
			{
				struct sockaddr_in clientaddr;
				socklen_t clientaddrlen = sizeof(clientaddr);
				//接受连接
				int connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddrlen);
				if(connfd < 0)
				{
					cout << "接受连接失败" << endl;
					continue;
				}
				//请求太多则拒绝
				if(clientcount >= 5)
				{
					string s = "请求过多已被服务器拒绝";
					send(connfd,s.c_str(),s.length(),0);
					close(connfd);
					continue;
				}
				//新连接接受后更新fds和client数组
				//client[connfd]就是新连接的连接描述符对应的客户端数据
				clientcount++;
				client[connfd].addr = clientaddr;
				SetNonblocking(connfd);
				fds[clientcount].fd = connfd;
				fds[clientcount].events = POLLIN | POLLRDHUP | POLLERR;
				fds[clientcount].revents = 0;
				cout << "接受新客户连接: " << inet_ntoa(client[connfd].addr.sin_addr) << endl; 
			}
			//处理每个描述符的错误事件
			else if(fds[i].revents & POLLERR)
			{
				cout <<fds[i].fd << "发生错误" << endl;
				char error[100];
				socklen_t len = sizeof(error);
				getsockopt(fds[i].fd,SOL_SOCKET,SO_ERROR,&error,&len);
				continue;
			}
			//如果客户端关闭连接
			else if(fds[i].revents & POLLRDHUP)
			{
				//回收相应描述符资源
				client[fds[i].fd] = client[fds[clientcount].fd];
				close(fds[i].fd);
				fds[i] = fds[clientcount];
				i--;
				clientcount--;
				cout << "一位客户端离开" << endl;
			}
			//如果数据可读
			else if(fds[i].revents & POLLIN)
			{
				int connfd = fds[i].fd;
				memset(client[connfd].readdata,'\0',sizeof(client[connfd].readdata));
				ret = recv(connfd,client[i].readdata,sizeof(client[i].readdata),0);
				cout << ret << endl;
				if(ret < 0)
				{
					if(errno != EAGAIN)
					{
						close(connfd);
						client[fds[i].fd] = client[fds[clientcount].fd];
						fds[i] = fds[clientcount];
						i--;
						clientcount--;
					}
				}				
				else if(ret == 0)
				{}
				else
				{
					//读到了数据则通知其他连接描述符准备写数据
					for(int j = 1;j <= clientcount;j++)
					{
						if(fds[j].fd == connfd)
							continue;
						fds[j].events |= ~POLLIN;
						fds[j].events |= POLLOUT;
						client[fds[j].fd].writedata = client[connfd].readdata;
					}
				}
			}
			//如果可写
			else if(fds[i].revents & POLLOUT)
			{
				cout << "write" << endl;
				int connfd = fds[i].fd;
				if(client[connfd].writedata.empty())
					continue;
				ret = send(connfd,client[connfd].writedata.c_str(),client[connfd].writedata.length(),0);
				cout << "send!!!!!!!!!" << endl;
				client[connfd].writedata.clear();
				fds[i].events |= ~POLLOUT;
				fds[i].events |= POLLIN;
			}

		}
	}

	delete[] client; 
	close(listenfd);
	for(int i = 0;i < 5;i++)
		close(fds[i].fd);
	return 0;
}