#include "Header.h"
#include "Client.h"
#include "Server.h"
#include "DES.h"
#include "AES.h"
#include <iomanip>
#include "LNA.h"
#include "RSA.h"
#include <exception>
#include <random>

#include "LNAShifts.h"

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



void LNAShifterTest(){
    const vector<uint8_t> TestModulus = {
        0xC4, 0xF8, 0xE9, 0xE1, 0x5D, 0xCA, 0xDF, 0x2B,
        0x96, 0xC7, 0x63, 0xD9, 0x81, 0x00, 0x6A, 0x64,
        0x4F, 0xFB, 0x44, 0x15, 0x03, 0x0A, 0x16, 0xED,
        0x12, 0x83, 0x88, 0x33, 0x40, 0xF2, 0xAA, 0x0E,
        0x2B, 0xE2, 0xBE, 0x8F, 0xA6, 0x01, 0x50, 0xB9,
        0x04, 0x69, 0x65, 0x83, 0x7C, 0x3E, 0x7D, 0x15,
        0x1B, 0x7D, 0xE2, 0x37, 0xEB, 0xB9, 0x57, 0xC2,
        0x06, 0x63, 0x89, 0x82, 0x50, 0x70, 0x3B, 0x3F
    };


    for (size_t shifts = 9; shifts < 10000; shifts++){
        LNA test(LNA::MSWF, TestModulus.begin(), TestModulus.end());


        LNAShifts shifter(test, shifts, "<<");

        //test <<= shifts;
        if (shifts == 64){
            cout << "s " << shifter << endl;
            cout << "t " << test << endl;
        }
        //cout << "s " << shifter << endl;
        //cout << "t " << test << endl;
        //assert(test == shifter);

        if (shifts == 65)
            cout << "";

        for (size_t i = 1; i < shifts; i++){
            //test >>= 1;
            shifter.goToShift(shifts - i);

            //if (!(test == shifter)){
            ///    cout << "s " << shifter << endl;
            ///    cout << "t " << test << endl;
            //}
            //assert(test == shifter);
        }

    }

    return ;
}
void rsaTest()
{
    const vector<uint8_t> TestModulus = {
        0xC4, 0xF8, 0xE9, 0xE1, 0x5D, 0xCA, 0xDF, 0x2B,
        0x96, 0xC7, 0x63, 0xD9, 0x81, 0x00, 0x6A, 0x64,
        0x4F, 0xFB, 0x44, 0x15, 0x03, 0x0A, 0x16, 0xED,
        0x12, 0x83, 0x88, 0x33, 0x40, 0xF2, 0xAA, 0x0E,
        0x2B, 0xE2, 0xBE, 0x8F, 0xA6, 0x01, 0x50, 0xB9,
        0x04, 0x69, 0x65, 0x83, 0x7C, 0x3E, 0x7D, 0x15,
        0x1B, 0x7D, 0xE2, 0x37, 0xEB, 0xB9, 0x57, 0xC2,
        0x06, 0x63, 0x89, 0x82, 0x50, 0x70, 0x3B, 0x3F
    };

    const  vector<uint8_t> TestPrivateKey = {
        0x8a, 0x7e, 0x79, 0xf3, 0xfb, 0xfe, 0xa8, 0xeb,
        0xfd, 0x18, 0x35, 0x1c, 0xb9, 0x97, 0x91, 0x36,
        0xf7, 0x05, 0xb4, 0xd9, 0x11, 0x4a, 0x06, 0xd4,
        0xaa, 0x2f, 0xd1, 0x94, 0x38, 0x16, 0x67, 0x7a,
        0x53, 0x74, 0x66, 0x18, 0x46, 0xa3, 0x0c, 0x45,
        0xb3, 0x0a, 0x02, 0x4b, 0x4d, 0x22, 0xb1, 0x5a,
        0xb3, 0x23, 0x62, 0x2b, 0x2d, 0xe4, 0x7b, 0xa2,
        0x91, 0x15, 0xf0, 0x6e, 0xe4, 0x2c, 0x41
    };

    const  vector<uint8_t> TestPublicKey = { 0x01, 0x00, 0x01 };



    RSA::Key key;

    key.modulus.vectorInit(LNA::MSWF, TestModulus.begin(), TestModulus.end());
    key.publicKey.vectorInit(LNA::MSWF, TestPublicKey.begin(), TestPublicKey.end());
    key.privateKey.vectorInit(LNA::MSWF, TestPrivateKey.begin(), TestPrivateKey.end());

    vector<uint8_t> message = { 'a', 'b', 'c' };
    vector<uint8_t> cypher, plaintext;

    RSA rsa(key);
    for (int i = 0; i < 50; i++){
        rsa.encrypt(message, cypher);
        rsa.decrypt(cypher, plaintext);

    }


    cout << "pt ";

    for (uint8_t* i = &plaintext[0]; i <= &plaintext[plaintext.size() - 1]; i++)
    {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)*i;
    }

}

void rsaComputeTest()
{
    //LNA e(79), d(1019), n(3337), m(688), c;

    /*RSA::compute(m, e, n, c);

    cout << "c " << c << endl;

    RSA::compute(c, d, n, m);

    cout << "m " << m << endl;*/
}

void httpTest()
{
    if(init()){
        Server server;
        //Client client;

        //client.request("http://prindal:pw@www.myProxy.com:8080/","http://www.msn.com");
        //client.request("http://localhost/path/to/dir.txt");
                
    }
}

void LNATest(){
    typedef LNA::T T;
    /* 0x0ee9a2efb6119e2c
    0xf7198a116438ebcd
    0xcde1e41efa5464d5*/
    /*vector<T> source1 = {7,0, (T)3 };
    vector<T> source2 = {(T)3 };*/
    //vector<T> source1 = { 0xae00feaa00b8b573, 0x46a99459b4ad9f11, 0xc1332568ceba5a70, 0xb598eef6ebbe7347 };
    //vector<T> source2 = { 0xa7b480b6b5f0b06c };
    //vector<T> source3 = { 1, 0x0ee9a2efb6119e2c, 0xf7198a116438ebcd, 0xcde1e41efa5464d5 };

    LNA num1;//(LNA::MSWF, source1);
    LNA num2;//(LNA::MSWF, source2);
    LNA quo;//(LNA::MSWF, source3), 
    LNA rem, sum;

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

            if (!(rem < num2)){
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
        size_t s2 = rand() % 4 + 1;
        num1.randomize(s1);
        num2.randomize(s2);
    }


}


int main(int argc, char** argv)
{
    LNATest();

    return 1;
}