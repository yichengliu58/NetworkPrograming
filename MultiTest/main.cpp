#include "base.h"
#include <iostream>
#include <cstdlib>
int main(int argc,char* argv[])
{
    /*if(argc < 3)
    {
        std::cout << "主函数参数太少" << std::endl;
        std::abort();
    }*/
    try
    {
        Socket listenfd(true);
//        std::cout << listenfd.Get() << std::endl;
        EndPoint server("192.168.1.100",/*std::atoi(argv[2])*/12347);
        ClientInfo client;
        listenfd.Bind(server);
        std::cout << "开始监听..." << std::endl;
        listenfd.Listen(5);
        Epoller poller;
        poller.Addfd(listenfd,Event::in, false);
        int res = poller.Wait(-1);
        //client.SetSocket(listenfd.Accept(client.GetEndPoint()));
        const std::vector<struct epoll_event>& e = poller.GetEventSet();
        for(auto& ff : e)
            std::cout << ff.data.fd << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}