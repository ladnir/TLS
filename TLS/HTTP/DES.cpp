#include "DES.h"
#include "Header.h"

#include <exception>
#include <random>
#include <cassert>

#define GET_BIT(array, bit) \
    (array[(int)(bit / 8)] & (0x80 >> (bit % 8)))

#define SET_BIT(array, bit) \
    (array[(int)(bit / 8)] |= (0x80 >> (bit % 8)))

#define CLEAR_BIT(array, bit) \
    (array[(int)(bit / 8)] &= ~(0x80 >> (bit % 8)))

#define EXPANSION_BLOCK_SIZE 6
#define PC1_KEY_SIZE 7
#define ROUND_KEY_SIZE 6

const int DES::ipTable[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

const int DES::fpTable[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

const int DES::epTable[48] = {
    32, 1 , 2 , 3 , 4 , 5 ,
    4 , 5 , 6 , 7 , 8 , 9 ,
    8 , 9 , 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

const int DES::pc1Table[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,

    63, 55, 47, 39, 31, 23, 15,
    7 , 62, 54, 46, 38, 30, 22,
    14, 6 , 61, 53, 45, 37, 29,
    21, 13, 5 , 28, 20, 12, 4
};

const int DES::pc2Table[48] = {
    14, 17, 11, 24, 1 , 5 , 3 , 28,
    15, 6 , 21, 10, 23, 19, 12, 4 ,
    26, 8 , 16, 7 , 27, 20, 13, 2 ,
    41, 52, 31, 37, 47, 55, 30, 40,
    51, 45, 33, 48, 44, 49, 39, 56,
    34, 53, 46, 42, 50, 36, 29, 32
};

const int DES::SBox[8][64] =
{
   {  /* S0 */
    14,  0,  4, 15, 13,  7,  1,  4,  2, 14, 15,  2, 11, 13,  8,  1,
     3, 10, 10,  6,  6, 12, 12, 11,  5,  9,  9,  5,  0,  3,  7,  8,
     4, 15,  1, 12, 14,  8,  8,  2, 13,  4,  6,  9,  2,  1, 11,  7,
    15,  5, 12, 11,  9,  3,  7, 14,  3, 10, 10,  0,  5,  6,  0, 13
    },
    {  /* S1 */
    15,  3,  1, 13,  8,  4, 14,  7,  6, 15, 11,  2,  3,  8,  4, 14,
     9, 12,  7,  0,  2,  1, 13, 10, 12,  6,  0,  9,  5, 11, 10,  5,
     0, 13, 14,  8,  7, 10, 11,  1, 10,  3,  4, 15, 13,  4,  1,  2,
     5, 11,  8,  6, 12,  7,  6, 12,  9,  0,  3,  5,  2, 14, 15,  9
    },
    {  /* S2 */
    10, 13,  0,  7,  9,  0, 14,  9,  6,  3,  3,  4, 15,  6,  5, 10,
     1,  2, 13,  8, 12,  5,  7, 14, 11, 12,  4, 11,  2, 15,  8,  1,
    13,  1,  6, 10,  4, 13,  9,  0,  8,  6, 15,  9,  3,  8,  0,  7,
    11,  4,  1, 15,  2, 14, 12,  3,  5, 11, 10,  5, 14,  2,  7, 12
    },
    {  /* S3 */
     7, 13, 13,  8, 14, 11,  3,  5,  0,  6,  6, 15,  9,  0, 10,  3,
     1,  4,  2,  7,  8,  2,  5, 12, 11,  1, 12, 10,  4, 14, 15,  9,
    10,  3,  6, 15,  9,  0,  0,  6, 12, 10, 11,  1,  7, 13, 13,  8,
    15,  9,  1,  4,  3,  5, 14, 11,  5, 12,  2,  7,  8,  2,  4, 14
    },
    {  /* S4 */
     2, 14, 12, 11,  4,  2,  1, 12,  7,  4, 10,  7, 11, 13,  6,  1,
     8,  5,  5,  0,  3, 15, 15, 10, 13,  3,  0,  9, 14,  8,  9,  6,
     4, 11,  2,  8,  1, 12, 11,  7, 10,  1, 13, 14,  7,  2,  8, 13,
    15,  6,  9, 15, 12,  0,  5,  9,  6, 10,  3,  4,  0,  5, 14,  3
    },
    {  /* S5 */
    12, 10,  1, 15, 10,  4, 15,  2,  9,  7,  2, 12,  6,  9,  8,  5,
     0,  6, 13,  1,  3, 13,  4, 14, 14,  0,  7, 11,  5,  3, 11,  8,
     9,  4, 14,  3, 15,  2,  5, 12,  2,  9,  8,  5, 12, 15,  3, 10,
     7, 11,  0, 14,  4,  1, 10,  7,  1,  6, 13,  0, 11,  8,  6, 13
    },
    {  /* S6 */
     4, 13, 11,  0,  2, 11, 14,  7, 15,  4,  0,  9,  8,  1, 13, 10,
     3, 14, 12,  3,  9,  5,  7, 12,  5,  2, 10, 15,  6,  8,  1,  6,
     1,  6,  4, 11, 11, 13, 13,  8, 12,  1,  3,  4,  7, 10, 14,  7,
    10,  9, 15,  5,  6,  0,  8, 15,  0, 14,  5,  2,  9,  3,  2, 12
    },
    {  /* S7 */
    13,  1,  2, 15,  8, 13,  4,  8,  6, 10, 15,  3, 11,  7,  1,  4,
    10, 12,  9,  5,  3,  6, 14, 11,  5,  0,  0, 14, 12,  9,  7,  2,
     7,  2, 11,  1,  4, 14,  1,  7,  9,  4, 12, 10, 14,  8,  2, 13,
     0, 15,  6, 12, 10,  9, 13,  0, 15,  3,  3,  5,  5,  6,  8, 11
    }
};

const int DES::pTable[32] = {
    16, 7 , 20, 21, 29, 12, 28, 17,
    1 , 15, 23, 26, 5 , 18, 31, 10,
    2 , 8 , 24, 14, 32, 27, 3 , 9 ,
    19,	13, 30,	6 , 22, 11, 4 , 25
};

DES::DES(const uint8_t* key, Mode mode) : BlockCypher(mode)
{
	mBlockSize = DES_BLOCK_SIZE;
	mKeySize = DES_KEY_SIZE;

	mKey = new uint8_t[8];
	memcpy(mKey, key, DES_KEY_SIZE);
}

DES::DES(const std::vector<uint8_t> key, Mode mode) : BlockCypher(mode)
{
	mBlockSize = DES_BLOCK_SIZE;
	mKeySize = DES_KEY_SIZE;

	mKey = new uint8_t[8];
	assert(key.size() == DES_KEY_SIZE);
	memcpy(mKey, &key[0], DES_KEY_SIZE);
}

DES::DES(Mode mode) : BlockCypher(mode) {}

DES::~DES()
{
	delete mKey;
}

void DES::blockOperate(const uint8_t* src,
                             uint8_t* dest,
                       const uint8_t* key,
                       const opType   operation)
{
    uint8_t ip[DES_BLOCK_SIZE];
    uint8_t ExpandedBlock[EXPANSION_BLOCK_SIZE];
    uint8_t subBlock[DES_BLOCK_SIZE / 2];
    uint8_t pBoxTarget[DES_BLOCK_SIZE / 2];
    uint8_t recombBox[DES_BLOCK_SIZE / 2];

    uint8_t permutedKey[PC1_KEY_SIZE];
    uint8_t roundKey[ROUND_KEY_SIZE];

    permute(ip, src, ipTable, DES_BLOCK_SIZE);
    permute(permutedKey, key, pc1Table, PC1_KEY_SIZE);
    

    for (int round = 0; round < 16; round++){

        permute(ExpandedBlock, ip + 4, epTable, EXPANSION_BLOCK_SIZE);

        if (operation == opType::Encrypt){
            rol(permutedKey);
            if (!(round <= 1 || round == 8 || round == 15))
            {
                rol(permutedKey);
            }
        }

        permute(roundKey, permutedKey, pc2Table, ROUND_KEY_SIZE);
       
        if (operation == opType::Decrypt){
            ror(permutedKey);
            if (!(round >= 14 || round == 7 || round == 0))
            {
                ror(permutedKey);
            }
        }

        xor(ExpandedBlock, roundKey, ROUND_KEY_SIZE);
        
        memset((void*)subBlock, 0, DES_BLOCK_SIZE / 2);

        subBlock[0] = SBox[0][(ExpandedBlock[0] & 0xfc) >> 2] << 4 | 
                      SBox[1][(ExpandedBlock[0] & 0x03) << 4 | (ExpandedBlock[1] & 0xf0) >> 4];

        subBlock[1] = SBox[2][(ExpandedBlock[1] & 0x0f) << 2 | (ExpandedBlock[2] & 0xc0) >> 6] << 4 |
                      SBox[3][(ExpandedBlock[2] & 0x3f)];

        subBlock[2] = SBox[4][(ExpandedBlock[3] & 0xfc) >> 2] << 4 |
                      SBox[5][(ExpandedBlock[3] & 0x03) << 4 | (ExpandedBlock[4] & 0xf0) >> 4];

        subBlock[3] = SBox[6][(ExpandedBlock[4] & 0x0f) << 2 | (ExpandedBlock[5] & 0xc0) >> 6] << 4 |
                      SBox[7][(ExpandedBlock[5] & 0x3f)];

        permute(pBoxTarget, subBlock, pTable, DES_BLOCK_SIZE / 2);

        memcpy((void*)recombBox , (void*)ip         , DES_BLOCK_SIZE / 2);
        memcpy((void*)ip        , (void*)(ip + 4)   , DES_BLOCK_SIZE / 2);
        xor   (recombBox        , pBoxTarget        , DES_BLOCK_SIZE / 2);
        memcpy((void*)(ip + 4)  , (void*)recombBox  , DES_BLOCK_SIZE / 2);
    }

    memcpy((void*)recombBox , (void*)ip         , DES_BLOCK_SIZE / 2);
    memcpy((void*)ip        , (void*)(ip + 4)   , DES_BLOCK_SIZE / 2);
    memcpy((void*)(ip + 4)  , (void*)recombBox  , DES_BLOCK_SIZE / 2);

    permute(dest, ip, fpTable, DES_BLOCK_SIZE);
}

void DES::rol(uint8_t* target)
{
    int carryLeft, carryRight;

    carryLeft = (target[0] & 0x80) >> 3;
    target[0] = (target[0] << 1) | ((target[1] & 0x80) >> 7);
    target[1] = (target[1] << 1) | ((target[2] & 0x80) >> 7);
    target[2] = (target[2] << 1) | ((target[3] & 0x80) >> 7);

    carryRight = (target[3] & 0x08) >> 3;
    target[3] = (((target[3] << 1) | ((target[4] & 0x80) >> 7)) & ~0x10) | carryLeft;

    target[4] = (target[4] << 1) | ((target[5] & 0x80) >> 7);
    target[5] = (target[5] << 1) | ((target[6] & 0x80) >> 7);
    target[6] = (target[6] << 1) | carryRight;
}

void DES::ror(uint8_t* target)
{
    int carryLeft,carryRight;

    carryLeft = (target[6] & 0x01) << 3;
    target[6] = (target[6] >> 1) | ((target[5] & 0x01) << 7);
    target[5] = (target[5] >> 1) | ((target[4] & 0x01) << 7);
    target[4] = (target[4] >> 1) | ((target[3] & 0x01) << 7);

    carryRight = (target[3] & 0x10 ) << 3;
    target[3] = (((target[3] >> 1) | ((target[2] & 0x01) << 7)) & ~0x08) | carryLeft;

    target[2] = (target[2] >> 1) | ((target[1] & 0x80) << 7);
    target[1] = (target[1] >> 1) | ((target[0] & 0x80) << 7);
    target[0] = (target[0] >> 1) | carryRight;
}

void DES::permute(uint8_t target[], const uint8_t src[], const int permuteTable[], int len)
{
    for (int i = 0; i < len * 8; i++){
        if (GET_BIT(src, (permuteTable[i] - 1))){
            SET_BIT(target, i);
        }
        else{
            CLEAR_BIT(target, i);
        }
    }
}

TripleDES::TripleDES(const uint8_t* key, Mode mode) : DES(mode)
{
	mBlockSize = DES_BLOCK_SIZE;
	mKeySize = DES_KEY_SIZE * 3;

	mKey = new uint8_t[DES_KEY_SIZE * 3];
	memcpy(mKey, key, mKeySize);
}
TripleDES::TripleDES(const std::vector<uint8_t> key, Mode mode) : DES(mode)
{
	mBlockSize = DES_BLOCK_SIZE;
	mKeySize = DES_KEY_SIZE * 3;

	mKey = new uint8_t[DES_KEY_SIZE * 3];
	assert(key.size() == mKeySize);
	memcpy(mKey, &key[0], mKeySize);
}

void TripleDES::blockOperate(const uint8_t* src,
							       uint8_t* dest,
							 const uint8_t* key,
							 const opType   operation)
{
	uint8_t* buffer1 = new uint8_t[DES_KEY_SIZE * 3];
	uint8_t* buffer2 = new uint8_t[DES_KEY_SIZE * 3];

	DES::blockOperate(src    , buffer1, key + DES_KEY_SIZE * 0, operation);
	DES::blockOperate(buffer1, buffer2, key + DES_KEY_SIZE * 1, operation);
	DES::blockOperate(buffer2, dest   , key + DES_KEY_SIZE * 2, operation);

	delete buffer1;
	delete buffer2;
}