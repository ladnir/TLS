#include "Server.h"
#include "Header.h"

using namespace std;

Server::Server()
    : mThread(&Server::run)
    , mClientAddrLength(sizeof(mClientAddr))
{

}


Server::~Server()
{
    if (mThread.joinable())
        mThread.join();
}

void Server::run()
{

    WSADATA wsaData;

    
    if ((mListenSocket = socket(PF_INET, SOCK_STREAM, 0)) != -1)
    {
            
    }
    else{
        cout << "Server, unable to create socket." << endl;
    }


}