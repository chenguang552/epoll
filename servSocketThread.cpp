#include "servSocketThread.hpp"
servSocket::servSocket()
{
    //构造函数  初始化socket
    socketInit();
}
servSocket::~servSocket()
{
    //析构函数  释放资源
    close(service);
    pthread_exit(NULL);
}

void servSocket::socketInit()
{
    service = socket(AF_INET, SOCK_STREAM, 0);

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
        exit(0);
    }

    // cout << "监听程序启动中!" << endl;
    //出错返回-1
    THREAD_NUM = 0;

    runService(_util);
}

void *servSocket::sendDate(void *listener)
{
    // pthread_mutex_lock(&mutex);
    //发送数据
    char bufferSend[256];
    memset(bufferSend, 0, sizeof(bufferSend));
    cin >> bufferSend;
    int sen = ::send(*(int *)listener, bufferSend, sizeof(bufferSend), 0);
    if (sen > 0)
        cout << "done!" << endl;
    // pthread_mutex_unlock(&mutex);
    return nullptr;
}

void *servSocket::recvDate(void *_util)
{
    pthread_detach(pthread_self());

    int listener = (*(util *)_util).listener;
    cout << "线程开启成功，准备接收数据..." << endl;
    char bufferRecv[256];
    while (1)
    {
        // pthread_mutex_lock(&mutex);
        memset(bufferRecv, 0, sizeof(bufferRecv));
        int len = ::recv(listener, bufferRecv, sizeof(bufferRecv), 0);
        if (len > 0)
        {
            cout << ">" << bufferRecv << endl;
        }
        else
        {
            sleep(1000);
        }
    }
    cout << "循环结束" << endl;
    close(listener);
    // pthread_exit(NULL);
    return nullptr;
}

void servSocket::runService(util)
{
    //引入epoll
    //创建epoll
    int epollfd = epoll_create1(EPOLL_CLOEXEC);

    //event
    struct epoll_event event;
    event.data.fd = service;

    //触发模式
    event.events = EPOLLIN;

    //将监听事件交给epoll管理
    epoll_ctl(epollfd, EPOLL_CTL_ADD, service, &event);

    //定义事件列表：：初始化32个
    vector<struct epoll_event> events(32);

    //调用epoll_wait()
    int eReadyN; //就绪事件数
    int listener;
    ssize_t readDate;
    eReadyN = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);

    sockaddr_in client_sockaddr;
    socklen_t length = sizeof(client_sockaddr);
    while (1)
    {
        eReadyN = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
        // nfds = epoll_wait(epfd,events,20,500);
        for (int i = 0; i < eReadyN; ++i)
        {
            if (events[i].data.fd == service) //有新的连接
            {
                listener = accept(service, (sockaddr *)&client_sockaddr, &length); //accept这个连接
                event.data.fd = listener;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &event); //将新的fd添加到epoll的监听队列中
            }
            else if (events[i].events & EPOLLIN) //接收到数据，读socket
            {
                char bufferRecv[64];
                memset(bufferRecv, 0, sizeof(bufferRecv));
                readDate = read(listener, bufferRecv, sizeof(bufferRecv)) ;//读
                if (readDate < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        continue;
                    else
                        break; //or return;
                }
                int _ptr;
                event.data.ptr = &_ptr; //md为自定义类型，添加数据
                event.events = EPOLLOUT | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, listener, &event); //修改标识符，等待下一个循环时发送数据，异步处理的精髓
            }
            else if (events[i].events & EPOLLOUT) //有数据待发送，写socket
            {
                // struct myepoll_data *md = (myepoll_data *)events[i].data.ptr; //取数据
                // listener = md->fd;
                // send(listener, md->ptr, strlen((char *)md->ptr), 0); //发送数据
                // ev.data.fd = listener;
                // ev.events = EPOLLIN | EPOLLET;
                // epoll_ctl(epfd, EPOLL_CTL_MOD, listener, &ev); //修改标识符，等待下一个循环时接收数据
            }
            else
            {
                //其他的处理
            }
        }
    }

    //启动服务器
    //调用接收数据与发送数据的函数

    // while (THREAD_NUM < 5)
    // {

    // sockaddr_in client_sockaddr;
    // socklen_t length = sizeof(client_sockaddr);

    // int listener = accept(service, (struct sockaddr *)&client_sockaddr, &length);
    // if (listener < 0)
    // {
    //     cout << "listenErr";
    //     exit(0);
    // }
    // _util.listener=listener;
    // int THREAD_NUM_C = THREAD_NUM;

    // cout << "创建[" << THREAD_NUM << "]号监听线程" << endl;
    // THREAD_NUM++;

    // THREAD_RECV[THREAD_NUM_C] = pthread_create(&threadRecv[THREAD_NUM_C], NULL, &recvDate, (void *)&_util);
    // if (THREAD_RECV[THREAD_NUM_C] == -1)
    //     cout << "recv_Thread_Err";

    // pthread_join(threadRecv[THREAD_NUM_C],NULL);
    // recvDate((void *)&listener);
    // }
}
