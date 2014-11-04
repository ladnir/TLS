#pragma once

#include "LNA.h"
#include "PRNG.h"

class PrimeGen
{
public:

	LNA mLowerBound, mUpperBound;
	PRNG mGenerator;

	uint32_t mTries;


	PrimeGen(const LNA& lowerBound, const LNA& upperBound, const PRNG& generator);
	~PrimeGen();

	bool getPrime(LNA& prime);

	static bool isPrime(const LNA&);
	static bool rabinMiller(const LNA&);

	static uint32_t primes[1000];

};
