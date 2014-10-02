#pragma once
#include "BlockCypher.h"
class AES :
    public BlockCypher
{
public:
    enum KeySize { K128Bits = 16, K192Bits = 24, K256Bits = 32};

    AES(const uint8_t* key, KeySize keySize, Mode mode);
    AES(const std::vector<uint8_t> key, Mode mode);
    ~AES();

protected:
    virtual void blockOperate(const uint8_t* src,
            uint8_t* dest,
            const uint8_t* key,
            const opType   operation);

    void init(const uint8_t* key, KeySize keySize);

    static void computeKeySchedule(const uint8_t* key, const KeySize keySize, uint8_t schedule[][4]);

    static void shiftRow(uint8_t state[][4]);
    static void subState(uint8_t state[][4]);
    static void addRoundKey(uint8_t state[][4], uint8_t word[][4]);
    static void subWord(uint8_t* word);
    static void rolWord(uint8_t* word);
    int mRounds;
    static const char sBox[16][16];

};

