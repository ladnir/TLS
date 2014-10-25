#include <string>
#include <stdint.h>
#include <vector>

std::string base64_encode(uint8_t const* bytes_to_encode, size_t byteLength);
std::string base64_decode(std::string const& encoded_string);

std::string base16_encode(uint8_t const* bytesToEncode, size_t byteLength);
std::vector<uint8_t> base16_decode(std::string const& encoded_string);
