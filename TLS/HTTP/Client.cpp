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

bool Client::parseProxy(const string& proxy)
{
    if (proxy != ""){
        mProxy = true;

        int hostStart, at, proxyPortStart;

        hostStart = proxy.find("//") + 2;
        at = proxy.find("@");
        if (at != -1)
        {
            int userNameStart = hostStart;
            int passwordStart = proxy.find(":", userNameStart) + 1;
            hostStart = at + 1;

            mProxyUName = proxy.substr(userNameStart, passwordStart - userNameStart - 1);
            mProxyPassword = proxy.substr(passwordStart, at - passwordStart);
        }
        else{
            mProxyUName = "";
            mProxyPassword = "";
        }

        mProxyHost = proxy.substr(hostStart);
        proxyPortStart = mProxyHost.find(":");

        if (proxyPortStart != -1){
            mProxyPort = stoi(mProxyHost.substr(proxyPortStart + 1, mProxyHost.size() - 1));
            mProxyHost = mProxyHost.substr(0, proxyPortStart);
        }
        else{
            mProxyPort = PROXY;
        }
    }
    else
    {
        mProxy = false;
    }

    return true;
}

bool Client::parseURL(const string& uri)
{
    int hostStart, pathStart, portStart;

    hostStart = uri.find("//") + 2;
    pathStart = uri.find("/", hostStart);

    mHost = uri.substr(hostStart, pathStart - hostStart);

    portStart = mHost.find(":");
    if (portStart != -1){
        mPort = stoi(mHost.substr(portStart + 1, mHost.size() - 1));
        mHost = mHost.substr(0, portStart);
    }
    else{
        mPort = HTTP;
    }

    if (pathStart != -1)
        mPath = uri.substr(pathStart + 1, uri.size() - pathStart - 1);
    else
        mPath = "";
    return true;
}

bool Client::httpGet()
{
    string request;

    if (mProxy) request = "GET HTTP://" + mHost + "/" + mPath + "HTTP/1.1\r\n";
    else        request = "GET " + mPath + " HTTP/1.1\r\n";

    if (send(mClientConnection, request.c_str(), request.size(), 0) == SOCKET_ERROR)
        return false;
        
    if (mProxy){
        request = "Proxy-Authentication: BASIC " + base64_encode((uint8_t *)(mProxyUName + ":" + mProxyPassword).c_str(), mProxyUName.size() + 1 + mProxyPassword.size());

        if (send(mClientConnection, request.c_str(), request.size(),0) == SOCKET_ERROR)
            return false;
    }

    request = "Host: " + mHost + "\r\n";
    if (send(mClientConnection, request.c_str(), request.size(), 0) == SOCKET_ERROR)
        return false; 

    request = "Connection: close\r\n\r\n";
    if (send(mClientConnection, request.c_str(), request.size(), 0) == SOCKET_ERROR)
        return false;
   
   
    
    return true;
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
        return true;    
    }
    cout << "couldnt open: C:\\Users\\peter\\Source\\Repos\\TLS\\out.html" << endl;
    return false;
}

bool Client::request(const string& uri)
{
    return request("",uri);
}

bool Client::request(const string& proxy,const string& uri)
{
    parseProxy(proxy);
    parseURL(uri);

    struct hostent* hostName;
    struct sockaddr_in hostAddress;

   
    mClientConnection = socket(PF_INET, SOCK_STREAM, 0);
    if (mClientConnection)
    {
        hostName = gethostbyname(mProxy ? mProxyHost.c_str() : mHost.c_str());
        if (hostName != nullptr)
        {
            hostAddress.sin_family = AF_INET;
            hostAddress.sin_port = htons(mProxy ? mProxyPort : mPort);
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