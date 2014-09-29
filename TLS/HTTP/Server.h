#pragma once
#include "Header.h"

#include <list>
#include <thread>

class Server
{
public:
    Server();
    ~Server();


private:

    std::thread mThread;
    void serverMain();

    void handleHttpRequest(int connectSocket);
    std::string readLine(int connectionSocket); 
    void buildErrorResponce(int connectionSocket);
    void buildSuccessResponce(int connectionSocket);

    std::list<std::thread> mServiceThreads;
};

