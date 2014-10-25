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
	typedef LNA::T T;
   /* 0x0ee9a2efb6119e2c
    0xf7198a116438ebcd
    0xcde1e41efa5464d5*/
    /*vector<T> source1 = {7,0, (T)3 };
    vector<T> source2 = {(T)3 };*/
    vector<T> source1 = { 0xae00feaa00b8b573, 0x46a99459b4ad9f11, 0xc1332568ceba5a70, 0xb598eef6ebbe7347 };
    vector<T> source2 = { 0xa7b480b6b5f0b06c };
    vector<T> source3 =                   {1, 0x0ee9a2efb6119e2c, 0xf7198a116438ebcd, 0xcde1e41efa5464d5 };
	
    LNA num1(source1);
    LNA num2(source2);
    LNA quo(source3),rem, sum;

    //LNA::show = true;

    /*quo *= num2;
    cout << "num1 " << num1 << endl;
    cout << "num2 " << num2 << endl;

    cout << "sum  " << quo << endl;
    assert(quo == num1);
    */
    int i = 0;
    while (++i){
        //num1 >>= 3;
        if (!num2.isZero()){

            //if (i == 6)
            //LNA::show = true;

            LNA::division(num1, num2, quo, rem); 

            sum.copy(quo);
            sum *= num2;
            sum += rem;

            if(!(rem < num2)){
                cout << "#" << i << endl;
                cout << "num1 " << num1 << endl;
                cout << "num2 " << num2 << endl;

                cout << "q " << quo << endl;
                cout << "r " << rem << endl;

                cout << "sum  " << sum << endl;
                cout << "num1 " << num1 << endl;
                cout << endl;

                assert(0 && "(rem < num2");
                throw new std::exception();
            }
            if (!(sum == num1)){
                cout << "#" << i << endl;
                cout << "num1 " << num1 << endl;
                cout << "num2 " << num2 << endl;

                cout << "q " << quo << endl;
                cout << "r " << rem << endl;

                cout << "sum  " << sum << endl;
                cout << "num1 " << num1 << endl;
                cout << endl;

                assert(0 && "sum == num1");
                throw new std::exception();
            }
           // }
        }
        size_t s1 = rand() % 8 + 1;
        size_t s2 = rand() %4 + 1;
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