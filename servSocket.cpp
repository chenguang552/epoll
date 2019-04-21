#include "servSocket.hpp"
servSocket::servSocket()
{
    //构造函数  初始化socket
    socketInit();
}
servSocket::~servSocket()
{
    //析构函数  释放资源
    close(service);
}

void servSocket::setSocketNoBlocking(int socketFd)
{

    int opts = fcntl(socketFd, F_GETFL);

    if (opts < 0)
    {
        cout << "F_GETFL_Err";
    }

    opts = (opts | O_NONBLOCK);
    if (fcntl(socketFd, F_SETFL, opts) < 0)
    {
        cout << "F_SETFL_Err";
    }
}

void servSocket::connectSocket(int _service, int _epollfd)
{
    //接收参数
    sockaddr_in client_sockaddr;
    socklen_t length = sizeof(client_sockaddr);

    int _listener = accept(_service, (sockaddr *)&client_sockaddr, &length); //accept这个连接

    if (_listener < 0)
    {
        cout << "listenErr" << endl;
        return;
    }

    cout << "新增客户端连接." << endl;

    setSocketNoBlocking(_listener); //设置socket为非阻塞

    //event
    struct epoll_event _event;
    _event.data.fd = _listener;
    //触发模式
    _event.events = EPOLLIN | EPOLLET;

    //将监听事件交给epoll管理
    if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listener, &_event) == -1)
    {
        close(_listener);
        cout << "addEpollErr" << endl;
        return;
    }
}

bool servSocket::readSocket(int _listener, int _epollfd)
{
    char bufferRecv[64];
    memset(bufferRecv, 0, sizeof(bufferRecv));
    int readLenth; //读
    cout << ">";
    while ((readLenth = ::read(_listener, bufferRecv, sizeof(bufferRecv))) > 0)
    {
        cout << bufferRecv;
    }
    cout << endl;

    //判断
    if (readLenth == 0)
    {
        return true;
    }
    else if (readLenth == -1 && errno != EAGAIN)
    {
        struct epoll_event _event;
        _event.data.fd = _listener;
        //_event.events = EPOLLOUT | EPOLLET;  //若写在循环中，则将原本监听的读事件改为写事件
        //epoll_ctl(_epollfd, EPOLL_CTL_MOD, _listener, &_event); //修改标识符，等待下一个循环时发送数据
        cout << "client down!" << endl;
        epoll_ctl(_epollfd, EPOLL_CTL_DEL, _listener, &_event); //连接断开，删除事件
        return false;
    }
    return true;
}

void servSocket::socketInit()
{
    service = socket(AF_INET, SOCK_STREAM, 0);
    if (service == -1)
    {
        cout << "create_Socket_Err" << endl;
        exit(0);
    }
    //设置非阻塞模式
    setSocketNoBlocking(service);

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(9999);
    //server_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(service, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) < 0)
    {
        cout << "bindErr";
        exit(0);
    }
    if (listen(service, 5) == -1)
    {
        cout << "listenErr";
        close(service);
        exit(0);
    }

    // cout << "监听程序启动中!" << endl;
    //出错返回-1

    runService();
}

void servSocket::runService()
{
    //引入epoll
    //创建epoll
    int epollfd = epoll_create1(EPOLL_CLOEXEC);

    //定义事件列表：：初始化32个
    vector<struct epoll_event> events(32);
    //eReadyN=就绪事件数
    int eReadyN;
    //event
    struct epoll_event event;
    event.data.fd = service;
    //触发模式
    event.events = EPOLLIN | EPOLLET;

    //将监听事件交给epoll管理
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, service, &event) == -1)
    {
        close(service);
        cout << "add_first_epoll_Err" << endl;
        return;
    }

    // int listener;
    ssize_t readDate;
    //eReadyN=就绪事件数
    //int eReadyN = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);

    // sockaddr_in client_sockaddr;
    // socklen_t length = sizeof(client_sockaddr);
    while (1)
    {
        //eReadyN=就绪事件数
        eReadyN = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
        // nfds = epoll_wait(epfd,events,20,500);
        for (int i = 0; i < eReadyN; i++)
        {
            if (events[i].data.fd == service) //有新的连接
            {
                connectSocket(service, epollfd);
            }
            else if (events[i].events & EPOLLIN) //接收到数据，读socket
            {
                if (readSocket(events[i].data.fd, epollfd))
                {
                    //消息到达，返回已读信息
                    int client_back = ::write(events[i].data.fd, "msg done!", 9);
                    if (client_back <= 0)
                        cout << "client_back_Err" << endl;
                }
            }
            else if (events[i].events & EPOLLOUT) //有数据待发送，写socket
            {
            }
            else
            {
                //其他的处理
            }
        }
    }
}