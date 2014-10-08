#pragma once
#include "LNA.h"

template<class T>
class BitIterator
{
public:
	//BitIterator(const FiniteField<T>&);
	BitIterator(const LNA<T>&);

	void goToBit(const int&);
	void goToMSB();
	void goToLSB();

	bool hasLesserBits() const;
	bool hasGreaterBits() const;

	inline bool operator*() const;
	void operator++(int);
	void operator--(int);

	bool operator<=(const BitIterator<T>&) const;
	bool operator>=(const BitIterator<T>&) const;
	bool operator==(const BitIterator<T>&) const;

	void flipBit();

private:
	//const FiniteField<T>& mField;
	const LNA<T>& mNumber;

	T mMask;
	int mWordIdx;
	int mBitIdx;
};

#include <windows.h> // WinApi header

template<class T>
BitIterator<T>::BitIterator(const LNA<T>& number)
: mNumber(number)
, mWordIdx(0)
, mBitIdx(0)
, mMask(1)
{}

template<class T>
void BitIterator<T>::goToBit(const int& idx)
{
	mBitIdx = idx % mNumber.mWordSize;
	mWordIdx = idx / mNumber.mWordSize;

	mMask <<= mBitIdx;
}

template<class T>
void BitIterator<T>::goToMSB()
{
	goToBit(mNumber.mWordSize * mNumber.mWordCount - 1);
}

template<class T>
void BitIterator<T>::goToLSB()
{
	mBitIdx = 0;
	mWordIdx = 0;
	mMask = 1;
}

template<class T>
bool BitIterator<T>::hasLesserBits() const
{
	return   mWordIdx == 0
		&& mBitIdx == 0;
}

template<class T>
bool BitIterator<T>::hasGreaterBits() const
{
	return   mWordIdx == (mNumber.mWordCount - 1)
		&& mBitIdx == (mNumber.mWordSize - 1);
}

template<class T>
bool BitIterator<T>::operator*() const
{
	return ((mNumber[mWordIdx] & mMask) != 0);
}

template<class T>
void BitIterator<T>::operator++(int)
{
	if (mBitIdx != mNumber.mWordSize - 1){
		// shift the mask inside a word
		mBitIdx++;
		mMask <<= 1;
	}
	else{
		// shift the mask to the start 
		//	of the next work
		mWordIdx++;
		mBitIdx = 0;
		mMask = 1;
	}
	//__asm{ rol mMask, 1 }
}

template<class T>
void BitIterator<T>::operator--(int)
{
	if (mBitIdx != 0){
		// shift the mask inside a word
		mBitIdx--;
		mMask >>= 1;
	}
	else{
		// shift the mask to the start 
		//	of the next work
		mWordIdx--;
		mBitIdx = mNumber.mWordSize - 1;
		mMask = (T)(1 << (sizeof(T)* 8 - 1));
	}
	//__asm{ ror mMask, 1 }
}

template<class T>
bool BitIterator<T>::operator<=(const BitIterator<T>& rhs) const
{
	if (mWordIdx < rhs.mWordIdx) return true;
	if (mWordIdx == rhs.mWordIdx && mBitIdx <= rhs.mBitIdx) return true;
	return false;
}

template<class T>
bool BitIterator<T>::operator>=(const BitIterator<T>& rhs) const
{
	if (mWordIdx > rhs.mWordIdx) return true;
	if (mWordIdx == rhs.mWordIdx && mBitIdx >= rhs.mBitIdx) return true;
	return false;
}

template<class T>
bool BitIterator<T>::operator==(const BitIterator<T>& rhs) const
{
	if (mWordIdx == rhs.mWordIdx && mBitIdx == rhs.mBitIdx) return true;
	return false;
}


template<class T>
void BitIterator<T>::flipBit()
{
	mNumber[mWordIdx] ^= mMask;
}

template<class T>
ostream& operator << (ostream& stream, const BitIterator<T>& bitIter)
{
	BitIterator<T> scanner(bitIter);
	BitIterator<T> lsb(bitIter);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	scanner.goToMSB();
	lsb.goToLSB();
	while (scanner >= lsb)
	{
		if (scanner == bitIter) SetConsoleTextAttribute(hConsole, 2);

		stream << *scanner;

		if (scanner == bitIter) SetConsoleTextAttribute(hConsole, 7);
		scanner--;
	}
	stream << "'";

	return stream;
}