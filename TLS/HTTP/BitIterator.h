#pragma once

template<class T>
class BitIterator
{
public:
	//BitIterator(const FiniteField<T>&);
	BitIterator(const LNA<T>&);

	void goToBit(const int&);
	void goToMSB();
	void goToLSB();

    bool isInRnage() const;
	//bool hasLesserBits() const;
	//bool hasGreaterBits() const;

    inline bool operator*() const;
    BitIterator<T>& operator++();
    BitIterator<T>& operator--();

	bool operator<=(const BitIterator<T>&) const;
	bool operator>=(const BitIterator<T>&) const;
	bool operator==(const BitIterator<T>&) const;

	void flipBit();

//private:
	//const FiniteField<T>& mNumber;
	const LNA<T>& mNumber;

	T mMask;
	int mWordIdx;
	int mBitIdx;
};


template<class T>
BitIterator<T>::BitIterator(const LNA<T>& field)
: mNumber(field)
, mWordIdx(0)
, mBitIdx(0)
, mMask(1)
{}

template<class T>
void BitIterator<T>::goToBit(const int& idx)
{
    mBitIdx = idx % mNumber.mWordSize;
    mWordIdx = idx / mNumber.mWordSize;
    
    mMask = ((T)1) << mBitIdx;
    assert(mMask);
    assert(mWordIdx < mNumber.mWordCount);
}

template<class T>
void BitIterator<T>::goToMSB()
{
    goToBit(mNumber.mWordCount * mNumber.mWordSize - 1);
}

template<class T>
void BitIterator<T>::goToLSB()
{
    mBitIdx = 0;
    mWordIdx = 0;
    mMask = 1;
}

template<class T>
bool BitIterator<T>::isInRnage() const
{
	return (mBitIdx >= 0 &&
		    mWordIdx >= 0 && 
		    mBitIdx < mNumber.mWordSize &&
			mWordIdx < mNumber.mWordCount);
}

//template<class T>
//bool BitIterator<T>::hasLesserBits() const
//{
//    return   mWordIdx == 0
//        && mBitIdx == 0;
//}
//
//template<class T>
//bool BitIterator<T>::hasGreaterBits() const
//{
//    return   mWordIdx == (mNumber.mWordCount - 1)
//        && mBitIdx == (mNumber.mWordSize - 1);
//}

template<class T>
bool BitIterator<T>::operator*() const
{
    assert(mMask);
    return ((mNumber[mWordIdx] & mMask) != 0);
}

template<class T>
BitIterator<T>& BitIterator<T>::operator++()
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

    return *this;
}

template<class T>
BitIterator<T>& BitIterator<T>::operator--()
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
        mMask = (T)(1 << (mNumber.mWordSize - 1));
    }
    //__asm{ ror mMask, 1 }

    return *this;
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
std::ostream& operator << (std::ostream& stream, const BitIterator<T>& bitIter)
{
    BitIterator<T> scanner(bitIter);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    scanner.goToMSB();
    while (scanner.isInRnage())
    {
        if (scanner == bitIter) SetConsoleTextAttribute(hConsole, 2);

        stream << *scanner;

        if (scanner == bitIter) SetConsoleTextAttribute(hConsole, 7);
        scanner--;
    }
    stream << "'";

    return stream;
}