#ifndef SERVSOCKET
#define SERVSOCKET
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

class servSocket {
public:
    int service;  //创建本地监听的socket
    
    sockaddr_in server_sockaddr;
    
public:
    servSocket();
    ~servSocket();
    void runService();

private:
    void setSocketNoBlocking(int);
    void connectSocket(int,int);
    void socketInit();
    
    bool readSocket(int,int);
    bool sendSocket(int,int);
};

#endif // !SERVSOCKET
