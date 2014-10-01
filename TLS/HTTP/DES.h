#pragma once

#define DES_BLOCK_SIZE 8
#define DES_KEY_SIZE 8

#include <stdint.h>
#include <vector>
#include "BlockCypher.h"

class DES : public BlockCypher
{
public:
	DES(const uint8_t* key, Mode mode);
	DES(const std::vector<uint8_t> key, Mode mode);
    ~DES();

protected:	

	DES( Mode mode);

	virtual void blockOperate(const uint8_t* src,
                                    uint8_t* dest,
                              const uint8_t* key,
                              const opType   operation);

    static void permute(uint8_t target[], 
                        const uint8_t src[], 
                        const int permuteTable[], 
                        int len);

    static void rol(uint8_t* target);
    static void ror(uint8_t* target);

    // DES permutation tables
    static const int ipTable[64];
    static const int fpTable[64];
    static const int epTable[48];
    static const int pc1Table[56];
    static const int pc2Table[48];
    static const int SBox[8][64];
    static const int pTable[32];
};

class TripleDES : public DES
{
public:
	TripleDES(const uint8_t* key, Mode mode);
	TripleDES(const std::vector<uint8_t> key, Mode mode);
	//~TripleDES();

protected:	

	virtual void blockOperate(const uint8_t* src,
                                    uint8_t* dest,
                              const uint8_t* key,
                              const opType   operation);
};
