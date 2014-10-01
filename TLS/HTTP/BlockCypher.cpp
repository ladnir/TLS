#include "BlockCypher.h"
#include <cassert>

BlockCypher::BlockCypher(Mode mode)
{
	mMode = mode;
}

void BlockCypher::encrypt(const std::vector<uint8_t>& plainText,
						  const std::vector<uint8_t>& iv,
						        std::vector<uint8_t>& cypherText)
{
	assert(iv.size() == mBlockSize);

	size_t cypherLength = plainText.size() + mBlockSize - (plainText.size() % mBlockSize);
	cypherText.resize(cypherLength);

	encrypt(&plainText[0], plainText.size(), &iv[0], &cypherText[0]);
}

void BlockCypher::encrypt(const uint8_t* plainText,
	                      const size_t   plainTextLength,
	                      const uint8_t* iv,
	                            uint8_t* cypherText)
{
	int paddedLength = mBlockSize - (plainTextLength % mBlockSize) + plainTextLength;
	uint8_t* paddedPlaintext = new uint8_t[paddedLength];

	memset(paddedPlaintext, 0x0, paddedLength); // optimie this memset
	memcpy(paddedPlaintext, plainText, plainTextLength);
	paddedPlaintext[plainTextLength] = 0x80;

	chainOperate(paddedPlaintext, cypherText, iv, mKey, paddedLength / mBlockSize, mMode, Encrypt);

	delete paddedPlaintext;
}

void BlockCypher::decrypt(const std::vector<uint8_t>& cypherText,
						  const std::vector<uint8_t>& iv,
						        std::vector<uint8_t>& plainText)
{
	assert(iv.size() == mBlockSize);

	size_t plainTextLength;
	plainText.resize(cypherText.size());
	decrypt(&cypherText[0], cypherText.size(), &iv[0], &plainText[0], plainTextLength);

	plainText.resize(plainTextLength);
}

void BlockCypher::decrypt(const uint8_t*  cypherText,
	                      const size_t    cypherTextLength,
	                      const uint8_t*  iv,
	                            uint8_t*  plainText,
	                            uint32_t& plainTextLength)
{
	size_t blockCount = cypherTextLength / mBlockSize;

	chainOperate(cypherText, plainText, iv, mKey, blockCount, mMode, Decrypt);

	int i = 1;
	do{
		if (i > 8){
			throw new std::exception("bad padding exception, did not find 0x80 at the end.");
		}
		if (plainText[blockCount * mBlockSize - i++] != 0){
			throw new std::exception("bad padding exception, does not have 0x00 at the end");
		}
	} while (plainText[blockCount * mBlockSize - i] != 0x80);
	plainTextLength = blockCount * mBlockSize - i;
}

void BlockCypher::blockEncrypt(const uint8_t* plainText,
	                                 uint8_t* cypherText)
{
	blockOperate(plainText, cypherText, mKey, opType::Encrypt);
}

void BlockCypher::blockDecrypt(const uint8_t* cypherText,
	                                 uint8_t* plainText)
{
	blockOperate(cypherText, plainText, mKey, opType::Decrypt);

}


void BlockCypher::chainOperate(const uint8_t* src,
	                                 uint8_t* dest,
	                           const uint8_t* iv,
	                           const uint8_t* key,
	                           const uint32_t blockCount,
	                           const Mode     mode,
	                           const opType   operation)
{
	uint8_t* tempBlock = new uint8_t[mBlockSize];
	uint8_t* inputBlock =  new uint8_t[mBlockSize];

	size_t byteCount = blockCount * mBlockSize;

	switch (mode){
	case Mode::ECB:
		for (size_t i = 0; i < byteCount; i += mBlockSize)
		{
			memcpy(inputBlock, src + i, mBlockSize);
			blockOperate(inputBlock, dest + i, key, operation);
		}

		break;
	case Mode::CBC:

		memcpy(tempBlock, iv, mBlockSize);

		for (size_t i = 0; i < byteCount; i += mBlockSize)
		{
			if (operation == opType::Encrypt)
			{
				memcpy(inputBlock, src + i, mBlockSize);
				xor(inputBlock, tempBlock, mBlockSize);
				blockOperate(inputBlock, dest + i, key, operation);
				memcpy(tempBlock, dest + i, mBlockSize);
			}
			else{
				memcpy(inputBlock, src + i, mBlockSize);
				blockOperate(inputBlock, dest + i, key, operation);
				xor(dest + i, tempBlock, mBlockSize);
				memcpy(tempBlock, src + i, mBlockSize);
			}
		}
		break;
	default:

		throw new std::exception("Mode not supported.");
	}
}

void BlockCypher::xor(uint8_t* target, const uint8_t* src, int len)
{
	while (len--)
	{
		*target++ ^= *src++;
	}
}