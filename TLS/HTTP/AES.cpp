#include "AES.h"
#include <cassert>

const char AES::sBox[16][16] = 
{
    {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76},
    {0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0},
    {0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15},
    {0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75},
    {0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84},
    {0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF},
    {0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8},
    {0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2},
    {0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73},
    {0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB},
    {0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79},
    {0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08},
    {0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A},
    {0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E},
    {0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF},
    {0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}
};

AES::AES(const uint8_t* key, KeySize keySize, Mode mode) : BlockCypher(mode)
{
    init(key, keySize);
}   

AES::AES(const std::vector<uint8_t> key, Mode mode) : BlockCypher(mode)
{
    init(&key[0], (KeySize)key.size());
}

AES::~AES()
{
    delete mKey;
}


void AES::init(const uint8_t* key, KeySize keySize)
{
    assert(keySize == K128Bits || keySize == K192Bits || keySize == K256Bits);

    mKeySize = keySize;

    if (mKeySize == K128Bits){
        mRounds = 10;
    }
    else if (mKeySize = K192Bits){
        mRounds = 12;
    }
    else{
        mRounds = 14;
    }
    mKey = new uint8_t[(mRounds + 1) * mKeySize];

    computeKeySchedule(&key[0], (KeySize)mKeySize, (uint8_t(*)[4]) mKey);
}

void AES::computeKeySchedule(const uint8_t* key, 
                             const KeySize  keySize, 
                                   uint8_t  schedule[][4])
{
    int i;
    int keyWords = keySize >> 2;
    uint8_t rcon = 0x01;

    memcpy(schedule, key, keySize);
    for (int i = keySize; i < 4 * (keyWords + 7); i++)
    {
        memcpy(schedule[i], schedule[i-1], 4);
        if (!(i % keyWords))
        {
            rolWord(schedule[i]);
            subWord(schedule[i]);
            if (!(i % 36)){
                rcon = 0x1b;
            }

            schedule[i][0] ^= rcon;
            rcon <<= 1;
        }
        else if (keyWords > 6 && i % keyWords == 4){
            subWord(schedule[i]);
        }

        schedule[i][0] ^= schedule[i - keyWords][0];
        schedule[i][1] ^= schedule[i - keyWords][1];
        schedule[i][2] ^= schedule[i - keyWords][2];
        schedule[i][3] ^= schedule[i - keyWords][3];
    }
}


void AES::blockOperate(const uint8_t* src,
                             uint8_t* dest,
                       const uint8_t* key,
                       const opType   operation)
{
    
}

void AES::subState(uint8_t state[][4])
{
    for (int row = 0; row < 4; row++){
        subWord(state[row]);
    }
}

void AES::shiftRow(uint8_t state[][4])
{
    uint8_t temp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = temp;

    temp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = temp;
    temp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = temp;

    temp = state[3][3];
    state[3][3] = state[3][2];
    state[3][2] = state[3][1];
    state[3][1] = state[3][0];
    state[3][0] = temp;

}

void AES::subWord(uint8_t* word)
{
    for (int i = 0; i < 4; i++){
        word[i] = sBox[(word[i] & 0xF0) >> 4 ][word[i] & 0x0F];
    }
}

void AES::addRoundKey(uint8_t state[][4], uint8_t word[][4])
{
    for (int col = 0; col < 4; col++){
        for (int row = 0; row < 4; row++){
            state[row][col] ^= word[row][col];
        }
    }
}

void AES::rolWord(uint8_t* word)
{
    uint8_t temp = word[0];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;
}
