#pragma once

#include <thread>

class Server
{
public:
    Server();
    ~Server();


private:
    int mListenSocket;
    int mConnectSocket;

    int mOn = 1;

    struct sockaddr_in mLocalAddr;
    struct sockaddr_in mClientAddr;
    int mClientAddrLength;

    std::thread mThread;
    void run();
};

