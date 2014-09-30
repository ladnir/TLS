#pragma once

#define DES_BLOCK_SIZE 8
#define DES_KEY_SIZE 8

#include <stdint.h>

class DES
{
public:
    DES(const uint8_t* key);
    ~DES();

    enum Mode {ECB, CBC};

    void encrypt(const uint8_t* plainText,
                       uint8_t* cypherText,
			     const uint8_t*	iv,
                 const uint32_t byteLength,
                 const Mode     mode);

    void decrypt( const uint8_t*  cypherText,
				  const uint32_t  blockCount,
				        uint8_t*  plainText,
				  const uint8_t*  iv,
				        uint32_t& byteLength,
				  const Mode      mode);

    void blockEncrypt(const uint8_t* plainText, 
                            uint8_t* cypherText);

    void blockDecrypt(const uint8_t* cypherText, 
                            uint8_t* plainText);

private:
    enum opType {Encrypt, Decrypt};

    static void chainOperate(const uint8_t* src,
                                   uint8_t* dest,
							 const uint8_t* iv,
                             const uint8_t* key,
                             const uint32_t blockCount,
                             const Mode		mode,
                             const opType   operation);

    static void blockOperate(const uint8_t* src,
                                   uint8_t* dest,
                             const uint8_t* key,
                             const opType   operation);

    static void xor(uint8_t* target, 
                    const uint8_t* src, 
                    int len);

    static void permute(uint8_t target[], 
                        const uint8_t src[], 
                        const int permuteTable[], 
                        int len);

    static void rol(uint8_t* target);
    static void ror(uint8_t* target);

    uint8_t* mKey;


    // DES permutation tables
    static const int ipTable[64];
    static const int fpTable[64];
    static const int epTable[48];
    static const int pc1Table[56];
    static const int pc2Table[48];
    static const int SBox[8][64];
    static const int pTable[32];
};
