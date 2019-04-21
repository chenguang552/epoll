#include "clieSocket.hpp"
clieSocket::clieSocket()
{
    //构造函数  初始化socket
    socketInit();
}
clieSocket::~clieSocket()
{
    //析构函数  释放资源
    // close(*client);
    pthread_exit(NULL);
}

void clieSocket::socketInit()
{
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        cout << "socket创建失败";
        exit(0);
    }
    client_sockaddr.sin_family = AF_INET;
    client_sockaddr.sin_port = htons(9999);
    client_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    cout << "准备接入服务器!" << endl;
    length = sizeof(client_sockaddr);
    if (connect(client, (struct sockaddr *)&client_sockaddr, length) < 0)
    {
        cout << "connectErr";
        exit(0);
    }
    cout << "接入服务器成功!" << endl;
    runClient();
}

void *clieSocket::sendDate(void *client)
{
    //发送数据
    // cout << "线程开启成功，准备发送数据..." << endl;
    while (1)
    {
        // pthread_mutex_lock(&mutexC);
        char bufferSend[64];
        memset(bufferSend, 0, sizeof(bufferSend));
        cout << "输入将要发送的数据：";
        cin>>bufferSend;
        // fgets(bufferSend, sizeof(bufferSend), stdin); //从键盘获取数据存在buf
        // const char bufferSend[64]="hello world.";
        // if (strcmp(bufferSend, "exit\n") == 0)
        // {
        //     cout<<"退出"<<endl;
        //     exit(0);
        // }
        if (sizeof(bufferSend) == 0)
        {
            cout << "inputErr";
            exit(0);
        }
        // for(int i=0;i<sizeof(bufferSend);i++)
        //     bufferSend[i]='*';
        int sen = ::send(*(int *)client, bufferSend, sizeof(bufferSend), 0);
        if (sen > 0)
            cout << "done!" << endl;
        // usleep(1000);
        // pthread_mutex_unlock(&mutexC);
    }
    cout<<"循环结束"<<endl;
    close(*(int *)client);
    // pthread_exit(NULL);
    return nullptr;
}

void *clieSocket::recvDate(void *client)
{
    //接受数据
    while (1)
    {
        // pthread_mutex_lock(&mutexC);
        char bufferRecv[64];
        memset(bufferRecv, 0, sizeof(bufferRecv));
        int len = ::recv(*(int *)client, bufferRecv, sizeof(bufferRecv), 0);
        if (len > 0)
            cout << bufferRecv << endl;
        // pthread_mutex_unlock(&mutexC);
    }
    close(*(int *)client);
    return nullptr;
}

void clieSocket::runClient()
{
    //启动服务器
    //调用接收数据与发送数据的函数
    // int THREAD_SEND = pthread_create(&threadSend, NULL, &sendDate, (void *)&client);
    // if (THREAD_SEND == -1)
    //     cout << "send_Thread_Err";
    // cout<<"join..."<<endl;
    // pthread_join(THREAD_SEND, NULL);
    sendDate((void *)&client);
}
