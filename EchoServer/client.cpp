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
const int serverport = 56784;

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
		string msg;
		cin >> msg;
		char buf[1024];
		send(sockfd,msg.c_str(),msg.length(),0);
		cout << "sent " << endl;
		if(recv(sockfd,buf,sizeof(buf),0) == 0)
		{
			cout << "server closed" << endl;
			close(sockfd);
			exit(1);
		}
		else
			cout << buf << endl;
	}
	close(sockfd);
	return 0;
}