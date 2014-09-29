#include <string>

std::string base64_encode(uint8_t const* bytes_to_encode, unsigned int in_len);


std::string base64_decode(std::string const& encoded_string);