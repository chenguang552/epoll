#ifndef SERVSOCKETTHREAD
#define SERVSOCKETTHREAD
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <iostream>
#include <pthread.h>
#include<vector>
#include<sys/epoll.h>

using namespace std;
struct util{
    int server;
    int listener;
};
class servSocket {
public:
    int service;  //创建本地监听的socket
    util _util;
    pthread_t threadRecv[5];
    int THREAD_RECV [5];
    int THREAD_NUM=0;
    sockaddr_in server_sockaddr;
    
public:
    servSocket();
    ~servSocket();
    void runService(util);

    static void* sendDate(void*);
    static void* recvDate(void*);

private:
    void socketInit();
};

#endif // !SERVSOCKETTHREAD


/*
struct myevent_s  
{  
    int fd;  
    void (*call_back)(int fd, int events, void *arg);  
    int events;  
    void *arg;  
    int status; // 1: in epoll wait list, 0 not in  
    char buff[128]; // recv data buffer  
    int len, s_offset;  
    long last_active; // last active time  
}; 
*/