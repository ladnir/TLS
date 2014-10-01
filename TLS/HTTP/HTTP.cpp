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

	vector<uint8_t> vKey{ 15, 12, 10, 5, 4, 3, 2, 1, 15, 12, 10, 5, 4, 3, 2, 1, 15, 12, 10, 5, 4, 3, 2, 1 };
	vector<uint8_t> iv  { 22, 1 , 4 , 5, 7, 33, 23, 4 };

	string input = "this is a story about a girl, who cried a river and drowned the whole world. ";
	vector<uint8_t> data (input.begin(), input.end());

	vector<uint8_t> cypherText;
	vector<uint8_t> plainText;

	TripleDES myDES(vKey, DES::CBC);
	myDES.encrypt(data, iv, cypherText);
	myDES.decrypt(cypherText, iv, plainText);

	return 1;

    //if(init()){
    //    Server server;
    //    //Client client;

    //    //client.request("http://prindal:pw@www.myProxy.com:8080/","http://www.msn.com");
    //    //client.request("http://localhost/path/to/dir.txt");
    //            
    //}
}