#include "Server.h"
#include "Header.h"

//using namespace std;

Server::Server()
    : mThread(&Server::serverMain, this)
{
    
}


Server::~Server()
{
    if (mThread.joinable())
        mThread.join();
}

void Server::serverMain()
{
    int listenSocket;
    int connectSocket;

    char mOn = 1;
    
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    int clientAddrLength = sizeof(clientAddr);

    if ((listenSocket = socket(PF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR){
        std::cerr << "Server, unable to create socket." << std::endl;
        return;       
    }

    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &mOn, sizeof(mOn)) == SOCKET_ERROR){
        std::cerr << "Server unable to set socket options." << std::endl;
        return;
    }

    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(Port::HTTP);
    localAddr.sin_addr.s_addr = htonl( INADDR_LOOPBACK);

    if (bind(listenSocket, (struct sockaddr*) & localAddr, sizeof(localAddr)) == SOCKET_ERROR){
        std::cerr << "server Unable to bind to local address:" << WSAGetLastError() << std::endl;
        return;
    }

    if (listen(listenSocket, 5) == SOCKET_ERROR){
        std::cerr << "server Unable to listen to socket." << std::endl;
        return;
    }

    while ((connectSocket = accept( listenSocket, 
                                   (struct sockaddr*) &clientAddr, 
                                   &clientAddrLength))             != -1)
    {
        std::cout << "connection accepted from " << connectSocket << std::endl;

        mServiceThreads.emplace_back(&Server::handleHttpRequest, this, connectSocket);
    }

    if (connectSocket == SOCKET_ERROR){
        std::cerr << "server Unable to accept connection." << std::endl;
        return;
    }
}

void Server::handleHttpRequest(int connectionSocket)
{
    std::string requestLine;

    requestLine = readLine(connectionSocket);

    if (requestLine.substr(0, 3) != "GET"){
        std::cerr << "Service thread error, only supports GET" << std::endl;
        buildErrorResponce(connectionSocket);
    }
    else{
        buildSuccessResponce(connectionSocket);
    }

   /* std::string line;

    while ((line = readLine(connectionSocket)) != ""){
        std::cout << line << std::endl;
    }*/

    if (closesocket(connectionSocket) == SOCKET_ERROR){
        std::cerr << "Unable to cloase connection." << std::endl;
    }
}

std::string Server::readLine(int connectionSocket)
{
    std::string line;
    char c;
  
    while (recv(connectionSocket, &c, 1, 0) > 0){
        line += c;
        if (line.size() > 1 && line.substr(line.size() - 2) == "\r\n")
            return line.substr(0, line.size() -2);
    }
    return line;
}

void Server::buildErrorResponce(int connectionSocket)
{
    std::string responce = "HTTP/1.1 501 Error Occured\r\n\r\n";

    if (send(connectionSocket, responce.c_str(), responce.size(), 0) < (int)responce.size()){
        std::cerr << "Server, Trying to send responce." << std::endl;
    }
}

void Server::buildSuccessResponce(int connectionSocket)
{
    std::string responce;
    responce  = "HTTP/1.1 200 OK\r\n";
    responce += "Content-Length: 72\r\n";
    responce += "Connection: close\r\n";
    responce += "Content-Type: text/html\r\n";
    responce += "\r\n";
    responce += "<html><head><title>Test Page</title></head><body>Test Page</body></html>\r\n";

    if (send(connectionSocket, responce.c_str(), responce.size(), 0) < (int)responce.size()){
        std::cerr << "Server, Trying to send responce." << std::endl;
    }
    else{
        std::cout << responce << std::endl;
    }
}