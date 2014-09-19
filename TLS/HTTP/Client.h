#pragma once
#include "Header.h"

class Client
{
public:
    Client();
    ~Client();
public:
    bool request(const string& uri);
private:

    bool writeBody(const string& responce);
    bool splitResponce(const string& responce, string& header, string& body);

    void getResponce();
    bool httpGet();
    bool parseURL(const string& uri);

    SOCKET mClientConnection;
    std::string mPath, mHost, mResponce;

    
};

