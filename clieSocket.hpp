#ifndef CLIESOCKET
#define CLIESOCKET
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

using namespace std;

class clieSocket { 
public:
    int client;
    socklen_t length;
    pthread_t threadSend;
    
    sockaddr_in client_sockaddr;

    clieSocket();
    ~clieSocket();

    void runClient();

    static void* sendDate(void *);
    static void* recvDate(void *);

private:
    void socketInit();
};

#endif // !CLIESOCKET
