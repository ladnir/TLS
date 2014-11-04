#pragma once

#include <vector>

class PRNG
{
public:
	PRNG();
	PRNG(const std::vector<uint8_t>& seed);
	~PRNG();

	std::vector<uint8_t> getNBits(size_t bits);
	std::vector<uint8_t> getNBytes(size_t bytes);
};

