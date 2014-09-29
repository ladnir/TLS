#include "Header.h"
#include "Client.h"
#include "Server.h"
#include "DES.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

bool init()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
    {
        cout << "Server, unable to initialize WSA." << endl;
        return false;
    }
    return true;
}


int main(int argc, char** argv)
{

    uint8_t key[DES_KEY_SIZE] = { 8, 7, 6, 5, 4, 3, 2, 1 };
    uint8_t data[DES_BLOCK_SIZE] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t cypherText[DES_BLOCK_SIZE] = {};
    uint8_t plainText[DES_BLOCK_SIZE] = {};

    DES myDES(key);

    myDES.blockEncrypt(data,cypherText);
    myDES.blockDecrypt(cypherText, plainText);


    //if(init()){
    //    Server server;
    //    //Client client;

    //    //client.request("http://prindal:pw@www.myProxy.com:8080/","http://www.msn.com");
    //    //client.request("http://localhost/path/to/dir.txt");
    //            
    //}
}