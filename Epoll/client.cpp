#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <memory.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <cmath>
using namespace std;

int main(int argc,char** argv)
{
	string serveraddr = argv[1];
	int serverport = atoi(argv[2]);

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	inet_pton(AF_INET,serveraddr.c_str(),&addr.sin_addr);
	addr.sin_port = htons(serverport);

	int sockfd = socket(PF_INET,SOCK_STREAM,0);
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1)
		printf("connect failed %d\n",errno);
		//cout << "connect failed" << endl;
	else
	{
		int i = 4;
		char buf[1024];
		string msg;
		int maxfd;
		while(true)
		{
			memset(buf,'\0',sizeof(buf));
			cin >> msg;
			int ssss = send(sockfd,msg.c_str(),msg.length(),0);
			//int ret = recv(sockfd,buf,sizeof(buf),0);
			cout << ssss << endl;
		}
	}
	close(sockfd);
	return 0;
}