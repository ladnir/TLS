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

    static void mixColumn(uint8_t state[][4]);
    static void shiftRows(uint8_t state[][4]);
    static void subState(uint8_t state[][4], const uint8_t box[16][16]);
    static void addRoundKey(uint8_t state[][4], uint8_t word[][4]);

    static void invMixColumn(uint8_t state[][4]);
    static void invShiftRows(uint8_t state[][4]);

    static uint8_t dot(uint8_t x, uint8_t y);
    static void subWord(uint8_t* word, const uint8_t box[16][16]);
    static void rolWord(uint8_t* word);

    int mRounds;

    static const uint8_t sBox[16][16];
    static const uint8_t invSBox[16][16];

};

