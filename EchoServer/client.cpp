//echo client code
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
using namespace std;
const string serveraddr = "127.0.0.1";
const int serverport = 56786;

int main()
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	inet_pton(AF_INET,serveraddr.c_str(),&addr.sin_addr);
	addr.sin_port = htons(serverport);

	int sockfd = socket(PF_INET,SOCK_STREAM,0);
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1)
		//printf("connect failed %d\n",errno);
		cout << "connect failed" << endl;
	else
	{
		char buf[1024];
		char re[1024];
		string msg;
		int maxfd;
		fd_set readfd;

		FD_ZERO(&readfd);
		while(true)
		{
			FD_SET(STDIN_FILENO,&readfd);
			FD_SET(sockfd,&readfd);
			maxfd = STDIN_FILENO > sockfd ? STDIN_FILENO : sockfd;

			select(maxfd + 1,&readfd,NULL,NULL,NULL);
			if(FD_ISSET(sockfd,&readfd))
			{
				int ret = recv(sockfd,buf,sizeof(buf),0);
				if(ret == 0)
				{
					cout << "server is closed" << endl;
					close(sockfd);
					FD_CLR(sockfd,&readfd);
					exit(1);
				}
				cout << buf << endl;
			}
			if(FD_ISSET(STDIN_FILENO,&readfd))
			{
				int n = read(STDIN_FILENO,re,1024);
             	if (n  == 0)
             	{
                	FD_CLR(STDIN_FILENO,&readfd);
                	continue;
             	}
             	write(sockfd,re,n);
			}
		}
	}
	close(sockfd);
	return 0;
}