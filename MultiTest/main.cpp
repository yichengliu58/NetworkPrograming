#include "base.h"
#include <iostream>
#include <cstdlib>
#include <list>
int main(int argc,char* argv[])
{
    std::list<ClientInfo> Clients;
    /*if(argc < 3)
    {
        std::cout << "主函数参数太少" << std::endl;
        std::abort();
    }*/
    try
    {
        Socket listenfd(true);
        EndPoint server("192.168.1.100",/*std::atoi(argv[2])*/12348);
        listenfd.Bind(server);
        std::cout << "开始监听..." << std::endl;
        Epoller poller;
        poller.Addfd(listenfd,Event::in, true);
        listenfd.Listen(5);
        while(true)
        {
            const std::vector<struct epoll_event>& res = poller.Wait(-1);
            std::cout << "返回wait" << std::endl;
            for(auto& r : res)
            {
                if (r.data.fd == listenfd.Get())
                {
                    std::cout << "listen" << std::endl;
                    ClientInfo client;
                    client.SetSocket(listenfd.Accept(client.GetEndPoint()));
                    poller.Addfd(client.GetSocket(), Event::in, true);
                    Clients.push_back(client);
                }
                else if (r.events & EPOLLIN)
                {
                    std::cout << "listendfdsfsd" << std::endl;
                    std::list<ClientInfo>::iterator it = Clients.begin();
                    for (it;it != Clients.end();)
                        if(it->GetSocket().Get() == r.data.fd)
                        {
                            try
                            {
                                std::string data = it->GetSocket().ReadData();
                                std::cout << data << std::endl;
                                break;
                            }
                            catch (const std::logic_error &e)
                            {
                                Clients.erase(it);
                                break;
                            }
                        }
                }
                std::cout << "foreach" << std::endl;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}