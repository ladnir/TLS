#include "Client.h"
#include "Header.h"
#include <fstream>

using namespace std;

Client::Client()
{
}


Client::~Client()
{
}


#define MAX_GET_COMMAND 255;

using namespace std;
enum Port { Ping = 7, Time = 13, HTTP = 80 };

bool Client::parseURL(const string& uri)
{
    int hostStart, pathStart;

    hostStart = uri.find("//") + 2;
    pathStart = uri.find("/", hostStart);

    mHost = uri.substr(hostStart, pathStart - hostStart);

    if (pathStart != -1)
        mPath = uri.substr(pathStart + 1, uri.size() - pathStart - 1);
    else
        mPath = "";

    return true;
}

bool Client::httpGet()
{
    string request;

    request = "GET " + mPath + " HTTP/1.1\r\n";
    if (send(mClientConnection, request.c_str(), request.size(), 0) != -1){

        request = "Host: " + mHost + "\r\n";
        if (send(mClientConnection, request.c_str(), request.size(), 0) != -1){

            request = "Connection: close\r\n\r\n";
            if (send(mClientConnection, request.c_str(), request.size(), 0) != -1){

                return true;
            }
        }
    }
    return false;
}

void Client::getResponce()
{
    const int bufSize = 1000;
    char buffer[bufSize + 1];
    int recieved;

    while ((recieved = recv(mClientConnection, buffer, bufSize, 0)) > 0){
        buffer[recieved] = 0;
        mResponce += buffer;
    }

}

bool Client::splitResponce(const string& responce, string& header, string& body)
{
    int split = responce.find("\r\n\r\n");

    header = responce.substr(0, split);
    body = responce.substr(split + 4, -1);
    return true;
}

bool  Client::writeBody(const string& responce)
{
    string header, body;

    splitResponce(responce, header, body);

    cout << "HEADER\n" << header << endl;
    cout << "BODY\n" << body << endl;

    fstream bodyFile;
    bodyFile.open("C:\\Users\\peter\\Source\\Repos\\TLS\\out.html", ios::out);

    if (bodyFile.good())
    {
        bodyFile << body;
        bodyFile.close();
    }
    else
    {
        cout << "couldnt open: C:\\Users\\peter\\Source\\Repos\\TLS\\out.html" << endl;
    }
}

bool Client::request(const string& uri)
{
    //string uri("http://www.google.com");

    parseURL(uri);

    struct hostent* hostName;
    struct sockaddr_in hostAddress;

   
    mClientConnection = socket(PF_INET, SOCK_STREAM, 0);
    if (mClientConnection)
    {
        hostName = gethostbyname(mHost.c_str());
        if (hostName != nullptr)
        {
            hostAddress.sin_family = AF_INET;
            hostAddress.sin_port = htons(Port::HTTP);
            memcpy(&hostAddress.sin_addr, hostName->h_addr_list[0], sizeof(struct in_addr));

            if (connect(mClientConnection, (struct sockaddr*)&hostAddress, sizeof(hostAddress)) != -1)
            {
                httpGet();
                getResponce();

                writeBody(mResponce);

            }
            else{
                cout << "Unable to connect to host" << endl;
            }
        }
        else{
            cout << "Could not find Host name: " << mHost << endl;
        }
    }
    else{
        cout << "Couldnt creat a socket" << endl;
    }

    return true;
}