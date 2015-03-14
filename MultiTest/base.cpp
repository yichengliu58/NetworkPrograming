#include "base.h"
#include <iostream>

int FileDescriptor::SetNonBlocking() const
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

int Socket::Accept(const EndPoint& end)
{
    int tmp;
    socklen_t size = end.GetRawAddrLen();
    if((tmp = ::accept(this->fd, (sockaddr*)&end.GetRawAddr(), &size)) == -1)
        throw std::runtime_error("无法接受连接！");
    else
        return tmp;
}

std::string Socket::ReadData() const
{
    char buf[MAX_BUFFER];
    memset(buf, '\0', MAX_BUFFER);
    int res = static_cast<int>(::recv(fd, buf, MAX_BUFFER, 0));
    if(res < 0)
        throw std::runtime_error("读取数据失败！");
    else if(res == 0)
        throw std::logic_error("客户端关闭了连接！");
    return std::string(buf);
}

void Socket::WriteData(const std::string& s)
{
    //循环写
    while(true)
    {
        int res = static_cast<int>(::write(fd, s.c_str(), s.length()));
        if(res < 0)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
                throw std::runtime_error("写入数据失败！");
        }
        else
            continue;
    }
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

void Epoller::Addfd(const FileDescriptor& fd, Event e,bool onet)
{
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
    if(::epoll_ctl(eventTable, EPOLL_CTL_ADD, fd.Get(), &event) == -1)
    {
        //std::cout << "errno = " << errno << std::endl;
        throw std::runtime_error("添加事件失败！");
    }
}

const std::vector<struct epoll_event>& Epoller::Wait(int millisecond,int& res)
{
    bzero(&*readyEvents.begin(), readyEvents.size());
    res = epoll_wait(eventTable, &*readyEvents.begin(),readyEvents.size(),millisecond);
    if(res > 0)
    {
        unsigned long size = readyEvents.size()*2 < MAX_EVENT ? readyEvents.size()*2 : MAX_EVENT;
        readyEvents.resize(size);
    }
    else
    {
        std::cout << "errno ; " << errno << std::endl;
        throw std::runtime_error("等待事件时出错！");
    }
    return readyEvents;

}





