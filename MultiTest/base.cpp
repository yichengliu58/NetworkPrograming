#include "base.h"
int FileDescriptor::SetNonBlocking()
{
    int old = fcntl(fd, F_GETFL);
    int new_ = old | O_NONBLOCK;
    fcntl(fd, F_SETFL,new_);
    return old;
}

EndPoint::EndPoint(const std::string& addr, int p)
{
    iner_addr.sin_family = AF_INET;
    inet_aton(addr.c_str(), &iner_addr.sin_addr);
    iner_addr.sin_port = htons(p);
}

void Socket::Bind(const EndPoint& end)
{
    if(::bind(this->fd,(sockaddr*)&end.GetRawAddr(), sizeof(end.GetRawAddr())) == -1)
        throw std::runtime_error("绑定失败！");
}

void Socket::Listen(int backlog)
{
    if(::listen(this->fd,backlog) == -1)
        throw std::runtime_error("监听套接字失败");
}

Socket Socket::Accept(const EndPoint& end)
{
    int tmp;
    socklen_t size = sizeof(end.GetRawAddr());
    if((tmp = ::accept(this->fd, (sockaddr*)&end.GetRawAddr(), &size)) == -1)
        throw std::runtime_error("无法接受连接！");
    else
        return Socket(tmp);
}

const Socket& ClientInfo::GetSocket() const
{
    return this->connfd;
}

std::string ClientInfo::GetIpAddr() const
{
    return this->clientaddr.GetAddrInString();
}

std::string ClientInfo::GetHostName() const
{
    struct hostent* p = ::gethostbyaddr(clientaddr.GetAddrInString().c_str(), static_cast<__socklen_t >(clientaddr.GetAddrInString().length()), AF_INET);
    return std::string(p->h_name);
}

void Epoller::Addfd(FileDescriptor& fd, Event e,bool onet) {
    struct epoll_event event;
    event.data.fd = fd.Get();
    switch (e) {
        case Event::in:
            event.events = EPOLLIN;
            break;
        case Event::out:
            event.events = EPOLLOUT;
            break;
        case Event::error:
            event.events = EPOLLERR;
            break;
        case Event::hangup:
            event.events = EPOLLHUP;
            break;
        default:
            break;
    }
    if (onet)
    {
        event.events |= EPOLLET;
        fd.SetNonBlocking();
    }
    epoll_ctl(eventTable, EPOLL_CTL_ADD, fd.Get(), &event);
}

int Epoller::Wait(int millisecond)
{
    //bzero(readyEvents, sizeof(readyEvents));
    int res = epoll_wait(eventTable, &*readyEvents.begin(), static_cast<int>(readyEvents.size()), millisecond);
    if(res > 0)
        readyEvents.resize(readyEvents.size()*2);
    return res;

}

const std::vector<struct epoll_event>& Epoller::GetEventSet()
{
    return readyEvents;
}





