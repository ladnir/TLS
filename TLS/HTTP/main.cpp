#include "Header.h"
#include "Client.h"
#include "Server.h"
#include "DES.h"
#include "AES.h"
#include <iomanip>
#include "LNA.h"

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
	typedef uint16_t T;

    vector<T> source1 = { (T)1, (T)-1 };
    vector<T> source2 = {(T)3 };
	
    LNA<T> num1(source1);
    LNA<T> num2(source2);

    cout << num1 << endl;
    cout << num2 << endl;
 	while (1){
		num1 *= num2;
        cout << num1 << endl;
	}
	return 1;

    //if(init()){
    //    Server server;
    //    //Client client;

    //    //client.request("http://prindal:pw@www.myProxy.com:8080/","http://www.msn.com");
    //    //client.request("http://localhost/path/to/dir.txt");
    //            
    //}
}