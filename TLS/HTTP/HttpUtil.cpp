
#include "Header.h"
#include <stdint.h>
#include <cassert>
#include <sstream>
#include <vector>
#include <exception>

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz"
										"0123456789+/";
std::string base64_encode(uint8_t const* bytes_to_encode, size_t in_len) {
    
    std::string ret;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i <4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';

    }

    return ret;

}

static inline bool is_base64(uint8_t c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string const& encoded_string) {
    size_t in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = (uint8_t)base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;

        for (j = 0; j <4; j++)
            char_array_4[j] = (uint8_t)base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}


std::string base16_encode(uint8_t const* bytesToEncode, size_t byteLength)
{
	static const std::string base16_chars = "0123456789ABCDEF";
	std::string encoding("0x");
	uint8_t idx;

    for (uint32_t i = 0; i < byteLength; i++)
	{
		idx = bytesToEncode[i] & 0xF0;
		encoding += base16_chars[idx];

		idx = bytesToEncode[i] & 0x0F;
		encoding += base16_chars[idx];
	}

	return encoding;
}

std::vector<uint8_t> base16_decode(std::string const& encoded_string) 
{
	std::vector<uint8_t> decode(encoded_string.size() >> 1);

	assert(encoded_string.size() % 2 == 0 ); // is even

	unsigned int idx = 0;
	if (encoded_string.size() >= 2){
		if (encoded_string[0] == '0' && encoded_string[1] == 'x')
		{
			idx += 2;
		}
	}
	uint8_t val;
	uint8_t c;
	while (idx < encoded_string.size())
	{
		val = 0;
		c = encoded_string[idx];
		if (     c >= (uint8_t)'0' && c <= (uint8_t)'9'){
			val = c - (uint8_t)'0';
		}
		else if (c >= (uint8_t)'A' && c <= (uint8_t)'F'){
			val = c - (uint8_t)'A' + 10;
		}
		else if (c >= (uint8_t)'a' && c <= (uint8_t)'f'){
			val = c - (uint8_t)'a' + 10;
		}
		else{
			throw new std::exception("Bad HEX(base16) encoding");
		}

		if (idx % 2 == 0){ // even
			decode[idx / 2] = val << 4;
		}
		else{ // odd
			decode[idx / 2] |= val;
		}
		idx++;
	}

	return decode;

	
}