#include "base.h"
#include <iostream>
#include <list>
#include <thread>

typedef std::shared_ptr<ClientInfo> ClientInfoPtr;

int main(int argc,char* argv[])
{
    std::list<ClientInfoPtr> Clients;
    if(argc < 3)
    {
        std::cout << "主函数参数太少" << std::endl;
        std::abort();
    }
    try
    {
        Socket listenfd(true);
        EndPoint server(argv[1]/*"192.168.1.100"*/,std::atoi(argv[2]));
        listenfd.Bind(server);
        std::cout << "开始监听..." << std::endl;
        Epoller poller;
        poller.Addfd(listenfd,Event::in, false);
        listenfd.Listen(5);
        while(true)
        {
            int num = 0;
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
            static int times = 1;
            const std::vector<struct epoll_event>& res = poller.Wait(-1,num);
            int i = 0;
            for(auto it = res.cbegin();i < num;++it,++i)
            {
                if(it->events & EPOLLIN)
                {
                    if (it->data.fd == listenfd.Get())
                    {
                        ClientInfoPtr client(new ClientInfo);
                        client->SetSocket(listenfd.Accept(client->GetEndPoint()));
                        std::cout << "接受新连接" << std::endl;
                        poller.Addfd(client->GetSocket(), Event::in, false);
                        Clients.push_back(client);
                    }
                    else
                    {
                        std::list<ClientInfoPtr>::iterator itor = Clients.begin();
                        for (itor; itor != Clients.end();)
                        {
                            if ((*itor)->GetSocket().Get() == it->data.fd)
                            {
                                try
                                {
                                    std::string data = (*itor)->GetSocket().ReadData();
                                    std::cout << data << std::endl;
                                    break;
                                }
                                catch (const std::logic_error &e)
                                {
                                    Clients.erase(itor);
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    std::cout << "发生了什么奇怪的事情" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}