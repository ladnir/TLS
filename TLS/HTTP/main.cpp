#include "Header.h"
#include "Client.h"
#include "Server.h"
#include "DES.h"
#include "AES.h"
#include <iomanip>
#include "LNA.h"
#include <exception>
#include <random>

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
#ifdef ST    
    string s("main");
    stackTrace st(s);
#endif
	typedef uint16_t T;

    /*vector<T> source1 = {7,0, (T)3 };
    vector<T> source2 = {(T)3 };*/
    vector<T> source1 = { 99};
    vector<T> source2 = { 19430 };
    vector<T> source3 = { 0, 0, 99 };
	
    LNA<T> num1(source1);
    LNA<T> num2(source2);
    LNA<T> quo(source3),rem;

    cout << "num1 " << num1 << endl;
    num1 -= quo;
    cout << "num1 " << num1 << endl;
    
    int i = 0;
    while (++i){
        //num1 >>= 3;
        if (!num2.isZero()){
            //if (num2[0] == 19430){// && num1[0] == 9832){
            //
            cout << "#" << i << endl;
            cout << "num1 " << num1 << endl;
            cout << "num2 " << num2 << endl;

            if (i == 2)
                LNA<T>::show = true;

            LNA<T>::division(num1, num2, quo, rem); 

            cout << "q " << quo << endl;
            cout << "r " << rem <<endl;

            quo *= num2;
            quo += rem;

            cout << "sum  " << quo <<  endl;
            cout << "num1 " << num1 << endl;
            cout << endl;

            assert(rem < num2);
            if (!(quo == num1)){
                assert(0 && "quo == num1");
            }
           // }
        }
        size_t s1 = rand() % 3 + 1;
        size_t s2 = rand() % 2 + 1;
        num1.randomize(s1);
        num2.randomize(s2);
        
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