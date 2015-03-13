#include "base.h"
#include <iostream>
#include <list>
#include <thread>

int main(int argc,char* argv[])
{
    std::list<ClientInfo> Clients;
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
            std::cout << "循环一次" << std::endl;
            const std::vector<struct epoll_event>& res = poller.Wait(-1);
            //std::cout << "返回wait " << res.capacity() << std::endl;
            for(auto& r : res)
            {
                std::cout << "event is " << r.events << std::endl;
                if(r.events & EPOLLIN)
                {
                    //std::cout << "获取事件" << std::endl;
                    if (r.data.fd == listenfd.Get())
                    {
                        ClientInfo client;
                        client.SetSocket(listenfd.Accept(client.GetEndPoint()));
                        std::cout << "接受新连接" << std::endl;
                        poller.Addfd(client.GetSocket(), Event::in, false);
                        Clients.push_back(client);
                    }
                    else
                    {
                        std::cout << "fuckyou!!!!" << std::endl;
                        /*std::list<ClientInfo>::iterator it = Clients.begin();
                        for (it; it != Clients.end();) {
                            if (it->GetSocket().Get() == r.data.fd) {
                                try {
                                    std::string data = it->GetSocket().ReadData();
                                    std::cout << data << std::endl;
                                    break;
                                }
                                catch (const std::logic_error &e) {
                                    Clients.erase(it);
                                    break;
                                }
                            }
                        }*/
                    }
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