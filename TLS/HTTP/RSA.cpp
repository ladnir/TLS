#include "RSA.h"

using namespace std;

const size_t RSA::paddingLen = 11;

void printVector(vector<uint8_t> output)
{
    vector<uint8_t>::const_iterator st = output.begin();
    vector<uint8_t>::const_iterator en = output.end();

    cout << "c1 ";
    for (const uint8_t* i = &(*st)
        ; i <= &(*(en - 1))
        ; i++)
    {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)*i;
    }
    cout << endl;
}

RSA::RSA(const Key& key)
{
    mKey.modulus = key.modulus;
    mKey.privateKey = key.privateKey;
    mKey.publicKey = key.publicKey;
}


RSA::~RSA()
{
}

size_t min(size_t n1, size_t n2)
{
    return (n1 > n2) ? n2 : n1;
}

void RSA::encrypt(const std::vector<uint8_t>& input,
                        std::vector<uint8_t>& output)
{
    size_t blockSize = mKey.modulus.byteLength();

    assert(blockSize > paddingLen);

    vector<uint8_t> paddedBlock;
    paddedBlock.reserve(blockSize);

    output.clear();

    for (vector<uint8_t>::const_iterator iter = input.begin();
                                         iter < input.end();
                                         iter += min(input.end() - iter, blockSize - paddingLen)){
        size_t contentSize = min(input.end() - iter, blockSize - paddingLen);

        // padding
        paddedBlock.clear();
        paddedBlock.push_back(0x00);
        paddedBlock.push_back(0x02);
        for (size_t i = 2; i < blockSize - contentSize - 1; i++)
            paddedBlock.push_back((uint8_t)i);// TODO make this random
        paddedBlock.push_back(0x00);

        // message
        paddedBlock.insert(paddedBlock.end(), iter, iter + contentSize);

        cout << "pb " ;

        for (uint8_t* i = &paddedBlock[0]; i <= &paddedBlock[paddedBlock.size() - 1]; i++)
        {
            std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)*i ;
        }

        cout << endl;

        LNA message(LNA::MSWF, paddedBlock);

        cout << "m  " << message << endl;
        LNA cypher;

        LNA::exponentiate(message, mKey.publicKey, mKey.modulus, cypher);

        cout << "c  " << cypher << endl;

        cypher.unload(LNA::MSWF, blockSize, output);

        printVector(output);

    }

}

void RSA::decrypt(const std::vector<uint8_t>& input,
                        std::vector<uint8_t>& output)
{
    size_t blockSize = mKey.modulus.byteLength();

    assert(blockSize > paddingLen);
    assert(input.size() % blockSize == 0);

    vector<uint8_t> paddedBlock;
    paddedBlock.reserve(blockSize);

    output.clear();

    for (vector<uint8_t>::const_iterator iter = input.begin();
                                         iter < input.end();
                                         iter += blockSize)
    {
        LNA cypher(LNA::MSWF, iter, iter + blockSize);
        LNA message;



        cout << "c  " << cypher << endl;

        LNA::secureExponentiate(cypher, mKey.privateKey, mKey.modulus, message);
        
        cout << "m  " << message << endl;

        message.unload(LNA::MSWF, blockSize, paddedBlock);

        cout << "pb ";

        for (uint8_t* i = &paddedBlock[0]; i <= &paddedBlock[paddedBlock.size() - 1]; i++)
        {
            std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint16_t)*i;
        }
        cout << endl;

        vector<uint8_t>::iterator blockIter = paddedBlock.begin();

        assert(*blockIter == 0x00);
        assert(*++blockIter == 0x02);
        while (*++blockIter != 0x00);
        
        *++blockIter;

        if (blockIter - paddedBlock.begin() != paddingLen)
            assert(iter + blockSize >= input.end());

        assert(blockIter - paddedBlock.begin() >= paddingLen);

        output.insert(output.end(), blockIter, paddedBlock.end());
    }

}

void RSA::compute(const LNA& m, const LNA& e, const LNA& n, LNA& c)
{
    LNA::exponentiate(m,e,n,c);
}
