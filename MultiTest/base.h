//封装简单的基础实用类
#include <netinet/in.h>
#include <bits/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <string.h>

const unsigned int MAX_EVENT(1024);
const unsigned int MAX_BUFFER(1024);

class EndPoint;
enum Event{in,out,error,hangup};

//文件描述符基类
class FileDescriptor
{
public:
    FileDescriptor()
            :fd(0)
    {}
    FileDescriptor(int f)
            :fd(f)
    {}
    virtual ~FileDescriptor()
    {
        close(fd);
    }
    //设置为非阻塞
    int SetNonBlocking() const;
    //获取描述符值
    int Get() const
    {
        return fd;
    }
protected:
    //描述符值
    int fd;
};




//封装基础套接字
class Socket : public FileDescriptor
{
public:
    //如果创建失败则抛出异常
    Socket() = default;
    Socket(bool on)
    {
        if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
            throw std::runtime_error("socket创建失败！");
    }
    //析构时关闭socket
    ~Socket() = default;
    //复制赋值运算符
    Socket& operator=(const Socket& other) = default;
    //拷贝构造函数也是默认的
    Socket(const Socket&) = default;
    //绑定地址结构体
    void Bind(const EndPoint&);
    //开始监听本socket
    void Listen(int);
    //接受连接
    Socket Accept(const EndPoint&);
    //在该套接字上读取数据至用户缓冲区
    std::string ReadData() const;
    //将数据写入套接字
    void WriteData(const std::string& );
};


//封装IP+端口号
class EndPoint
{
public:
    EndPoint()
    {
        bzero(&iner_addr, sizeof(iner_addr));
    }
    EndPoint(const std::string&,int);
    ~EndPoint() = default;

    //获取原始地址结构引用
    const struct sockaddr_in& GetRawAddr() const
    {
        return iner_addr;
    }
    //获取端口号
    int GetPort() const
    {
         return ::ntohs(iner_addr.sin_port);
    }
    //获取字符串地址
    std::string GetAddrInString() const
    {
        return std::string(inet_ntoa(iner_addr.sin_addr));
    }
private:
    struct sockaddr_in iner_addr;
};

//封装客户端信息
class ClientInfo
{
public:
    ClientInfo() = default;
    ClientInfo(Socket& s,EndPoint& e)
            :connfd(s),clientaddr(e)
    {
    }
    //外部设置socket描述符
    void SetSocket(const Socket& s)
    {
        connfd = s;
    }
    //外部设置endpoint
    void SetEndPoint(const EndPoint& e)
    {
        clientaddr = e;
    }
    //获取socket
    const Socket& GetSocket() const;
    //获取ip地址
    std::string GetIpAddr() const;
    //获取主机名
    std::string GetHostName() const;
    //获取储存的EndPoint
    const EndPoint& GetEndPoint() const
    {
        return clientaddr;
    }
private:
    Socket connfd;
    EndPoint clientaddr;
};

//封装epoll的过程
class Epoller
{
public:
    Epoller()
            :eventTable(0),readyEvents(1)
    {
        eventTable = ::epoll_create(5);
        if(eventTable <= 0)
            throw std::runtime_error("epoll事件表创建失败！");
    }
    Epoller(const Epoller&) = delete;
    ~Epoller()
    {
        close(eventTable);
    }

    //将指定描述符的指定事件添加进事件表
    void Addfd(const FileDescriptor& ,Event,bool);
    //开始等待
    const std::vector<struct epoll_event>& Wait(int);
private:
    //内核事件表描述符
    int eventTable;
    //每一次epoll_wait返回时填充的事件数组
    std::vector<struct epoll_event> readyEvents;
};

//封装一次TCP连接
class TcpConnection
{

};

/*//封装事件循环
class EventLoop
{
public:
    EventLoop()
            :isLoop(true),poller()
    {}
    ~EventLoop() = default;
    EventLoop(const EventLoop&) = delete;
    //开始循环
    void BeginLoop();
    //停止循环
    void StopLoop();
private:
    //是否中断
    bool isLoop;
    //用于事件循环的poller
    Epoller poller;
};*/




