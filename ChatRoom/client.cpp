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

#define BUFFERSIZE 64

using namespace std;

int main(int argc, char const *argv[])
{
	if(argc <= 2)
	{
		cout << "Usage: " << argv[0] << "IpAddr Port" << endl;
		exit(1);
	}
	const string ip = argv[1];
	int port = std::atoi(argv[2]);
	//创建服务器地址结构体
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET,ip.c_str(),&serveraddr.sin_addr);
	serveraddr.sin_port = htons(port);
	//创建连接socket描述符
	int sockfd = socket(PF_INET,SOCK_STREAM,0);
	assert(sockfd >= 0);
	//连接
	int ret = connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(ret < 0)
	{
		cout << "连接失败" << endl;
		close(sockfd);
		exit(1);
	}

	//在poll内部注册标准输入和连接描述符的可读事件
	pollfd fds[2];
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	fds[1].fd = sockfd;
	fds[1].events = POLLIN | POLLRDHUP;
	fds[1].revents = 0;

	// 接收数据缓冲区
	char buf[BUFFERSIZE];
	//构造管道
	int pipefd[2];
	ret = pipe(pipefd);
	assert(ret != -1);

	//开始循环轮询
	while(1)
	{
		ret = poll(fds,2,-1);
		if(ret < 0)
		{
			cout << "poll失败" << endl;
			exit(1);
		}
		//判断连接描述符上服务器是否关闭连接
		if(fds[1].revents & POLLRDHUP)
		{
			cout << "服务器关闭连接" << endl;
			close(sockfd);
			exit(1);
		}
		//判断连接描述符上是否有数据可读（服务器发送的数据）
		else if(fds[1].revents & POLLIN)
		{
			memset(buf,'\0',sizeof(buf));
			recv(fds[1].fd,buf,BUFFERSIZE - 1,0);
			cout << "fuckyou!!!!!!" << endl;
		}
		//判断标准输入是否有数据可读
		if(fds[0].revents & POLLIN)
		{
			//使用splice将用户输入数据直接移动到连接描述符上（使用管道）
			ret = splice(0,NULL,pipefd[1],NULL,32768,5);
			ret = splice(pipefd[0],NULL,sockfd,NULL,32768,5);

		}
	}
	close(sockfd);
	return 0;
}