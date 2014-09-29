#pragma once
#include "Header.h"

class Client
{
public:
    Client();
    ~Client();

    bool request(const std::string& uri);
    bool request(const std::string& proxy, const std::string& uri);

private:

    bool writeBody(const std::string& responce);
    bool splitResponce(const std::string& responce, std::string& header, std::string& body);

    void getResponce();
    bool httpGet();
    bool parseURL(const std::string& uri);
    bool parseProxy(const std::string& proxy);

    SOCKET mClientConnection;
    std::string mPath, mHost, mResponce;
    std::string mProxyHost, mProxyUName, mProxyPassword;

    bool mProxy;
    int mPort, mProxyPort;
    
};

