#pragma once

#include <stdint.h>
#include <vector>

class BlockCypher
{
public:
	enum Mode { ECB, CBC };

	BlockCypher(Mode mode);
	Mode mMode;

	// C++ style
	void encrypt(const std::vector<uint8_t>& plainText,
			     const std::vector<uint8_t>& iv,
			           std::vector<uint8_t>& cypherText);

	void decrypt(const std::vector<uint8_t>& cypherText,
			     const std::vector<uint8_t>& iv,
			           std::vector<uint8_t>& plainText);

	// C style
	void encrypt(const uint8_t* plainText,
                 const size_t   plainTextLength,
				 const uint8_t* iv,
                       uint8_t* cypherText);

	void decrypt(const uint8_t*  cypherText,
				 const size_t    cypherTextLength,
				 const uint8_t*  iv,
                       uint8_t*  plainText,
					   size_t&   plainTextLength);

	void blockEncrypt(const uint8_t* plainText,
		uint8_t* cypherText);

	void blockDecrypt(const uint8_t* cypherText,
		uint8_t* plainText);


protected:

	enum opType {Encrypt, Decrypt};
    virtual void blockOperate(const uint8_t* src,
                                    uint8_t* dest,
                              const uint8_t* key,
                              const opType   operation) = 0;

	static void xor(uint8_t* target, 
                    const uint8_t* src, 
                    int len);

	size_t mBlockSize;
	size_t mKeySize;
	uint8_t* mKey;

private:
    void chainOperate(const uint8_t* src,
                            uint8_t* dest,
	                  const uint8_t* iv,
                      const uint8_t* key,
                      const uint32_t blockCount,
                      const Mode	 mode,
                      const opType   operation);
};

