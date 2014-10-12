#pragma once
#include <stdint.h>
#include <vector>

template<class T>
class LNA
{
public:
    LNA(const std::vector<T>&);
	LNA(const LNA<T>&);
	LNA();
    ~LNA();

    static void add(LNA<T>& target,const LNA<T>& source);
	static void subtract(LNA<T>& target, const LNA<T>& source);

	static void multiply(const LNA<T>& multiplicand, 
						 const LNA<T>& multiplier, 
							   LNA<T>& product);

    static void division(const LNA<T>& dividend,
						 const LNA<T>& divisor,
							   LNA<T>& quotient,
							   LNA<T>& remainder);
	void resize(size_t);
	bool retract();
    void clear();

	T&      operator[]  (const size_t&)const;
    bool    operator>=  (const LNA<T>&)const;
    bool    operator==  (const LNA<T>&)const;
    LNA<T>& operator=   (const LNA<T>&);
    void    operator+=  (const LNA<T>&);
    void    operator*=  (const LNA<T>&);
	void    operator<<= (const size_t& bits);

    size_t mWordCount;
	size_t mWordSize;

    T* mNum;
};

#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>
#include "BitIterator.h"

template<class T>
LNA<T>::LNA(const std::vector<T>& source)
{
	mWordSize = sizeof(T) * 8;
	mWordCount = source.size();
	mNum = new T[mWordCount];

	for (size_t i = 0; i < mWordCount; i++){
		mNum[i] = source[mWordCount - i - 1];
	}

}

template<class T>
LNA<T>::LNA(const LNA<T>& src)
{
	mNum = new T[src.mWordCount];
	mWordCount = src.mWordCount;
	mWordSize = src.mWordSize;

	memcpy(mNum, src.mNum, src.mWordCount * src.mWordSize);
}
template<class T>
LNA<T>::LNA()
{
	mWordSize = sizeof(T);
	mWordCount = 1;
	mNum = new T[mWordCount]();
}

template<class T>
LNA<T>::~LNA()
{
    if (mNum != nullptr)
    	delete mNum;
}

template<class T>
void LNA<T>::resize(size_t size)
{
	assert(size > mWordCount);

	T* temp = mNum;
	mNum = new T[size](); // zero initialized

	memcpy(mNum , temp, mWordCount * sizeof(T)); // copy old value;

	delete temp;
	mWordCount = size;
}

template<class T>
bool LNA<T>::retract()
{
	size_t half = mWordCount / 2;
	for (size_t i = mWordCount - 1; i > half; i--){
		if (mNum[i])
			return false; // atleast half full
	}

	T* temp = mNum;
	mNum = new T[half];
	for (int i = 0; i < half; i++){
		mNum[i] = temp[i];
	}
	delete temp; 
	mWordCount = half;
	return true;
}

template<class T>
void LNA<T>::clear()
{
	memset(mNum, 0,mWordCount * mWordSize);
}

template<class T>
void LNA<T>::multiply(const LNA<T>& multiplicand, 
				      const LNA<T>& multiplier, 
					        LNA<T>& product)
{
    product.clear();
	LNA<T> multShift(multiplier);
	BitIterator<T> multIter(multiplicand);

	while (multIter.isInRnage()){
		if (*multIter){
			product += multShift;
			multShift <<= 1;
        }
		++multIter;
    } 
}

template<class T>
void LNA<T>::division(const LNA<T>& dividend,
					  const LNA<T>& divisor,
					        LNA<T>& quotient,
					        LNA<T>& remainder)
{
    assert(!divisor.isZero() && "Divide by zero error.");
    quotient.clear();
    if (dividend.isZero()){
        remainder.clear();
        return;
    }

    LNA<T> divisorShift(divisor);
    BitIterator<T> quotientIter(quotient);
    BitIterator<T> dividentIter(dividend);
    BitIterator<T> divisorIter(divisor);
    remainder.copy(dividend);

    size_t dividendLeadingZeros = 0;
    size_t divisorLeadingZeros = 0;

    dividentIter.goToMSB();
    while (*dividentIter == 0){
        --dividentIter;
        dividendLeadingZeros++;
    }

    divisorIter.goToMSB();
    while (*divisorIter == 0){
        --divisorIter;
        divisorLeadingZeros++;
    }

    size_t shifts = dividend.mWordCount * dividend.mWordSize
                  - divisor.mWordCount  * divisor.mWordSize
                  - dividendLeadingZeros 
                  + divisorLeadingZeros;

    quotientIter.goToBit(shifts);
    divisorShift <<= shifts;

    while (shifts != (size_t)-1){
        if (divisorShift <= remainder){
            remainder -= divisorShift;
            quotientIter.flipBit();
        }

        --quotientIter;
        divisorShift>>=1;
        shifts--;
    }
}

template<class T>
void LNA<T>::add(LNA<T>& target,const LNA<T>& source)
{
	if (target.mWordCount < source.mWordCount){
		target.resize(source.mWordCount);
	}

	T carry = 0;
	T t1;
	
	for (size_t idx = 0; idx < source.mWordCount; idx++)
	{
		if (source[idx] || carry){
			t1 = target[idx];
			target[idx] += source[idx] + carry;
			//cout << target << endl;
			carry = (target[idx] <= t1) ? 1 : 0;
		}
	}

	// target  ##############             
	// source  ######
	// cur pos       ^
	//
	// If we still have a carry, we have to place it in target's high words.

	if (carry){

		if (target.mWordCount == source.mWordCount){
			// expand target and set its top work to 1
			target.resize(target.mWordCount + 1);
			target[target.mWordCount - 1]++;
		}
		else{
			// keep applying the carry to target
			for (size_t i = source.mWordCount;i < target.mWordCount; i++){
				target[i]++;
				if (target[i]) // if its non zero, we can stop since there are no more carries
					return; // all done
			}

			// Need to resize to fit the last carry
			if (target[target.mWordCount - 1] == 0){
				target.resize(target.mWordCount + 1);
				target[target.mWordCount - 1] ++;
			}
		}
	}
}


template<class T>
void LNA<T>::subtract(LNA<T>& target, const LNA<T>& source)
{
	//assert(target >= source && "cant be negative");
	assert(target.mWordCount >= source.mWordCount && "NOT IMPLEMENTED: subtraction resulted in a negative number"); 
	T t1;

	for (size_t idx = source.mWordCount - 1; idx != (size_t) -1; idx--){
		t1 = target[idx];
		target[idx] -= source[idx];

		if (target[idx] > t1){
			// borrow from above
			size_t i;
			for (i = idx + 1; 1; i++){
				assert(i < target.mWordCount && "NOT IMPLEMENTED: subtraction resulted in a negative number"); // assert i >= 0

				if (target[i]){
					target[i] --; // found somewhere to borrow from.

					for (i--; i > idx ;i--){ // set all intermidiate values to the max value
						target[i] = (T)-1; //  target[i] = ffff...
					}
					break;
				}
			}
		}
	}

	target.retract();
}


template<class T>
T& LNA<T>::operator[](const uint32_t& idx) const
{
	assert(idx < mWordCount);
	return mNum[idx];
}

template<class T>
void LNA<T>::operator<<=(const size_t& shifts)
{
    assert(shifts < mWordCount * mWordSize);
    size_t s = shifts;
    size_t step = (s > mWordSize) ? mWordSize : s;

    size_t wordsAdded = shifts + (mWordSize - 1) / mWordSize; 
    
    for (size_t i = mWordCount - 1; mNum[i] == 0 && wordsAdded && i != (T)-1; i--) wordsAdded--;   
    if (wordsAdded) resize(mWordCount + wordsAdded);

    while (s){
        T shiftIn;
        for (size_t i = mWordCount - 1; i > 0; i--){
            mNum[i] <<= step;
            shiftIn = mNum[i - 1] >> (sizeof(T)* 8 - step);
            mNum[i] ^= shiftIn;
        }
        mNum[0] <<= step;
        s -= step;
        step = (s > mWordSize) ? mWordSize : s;
    }
}


template<class T>
bool LNA<T>::operator==(const LNA<T>& cmp) const
{
    size_t min = (mWordCount > cmp.mWordCount)? cmp.mWordCount : mWordCount;
    size_t i;

    for (i = 0; i < min; i++){
        if (mNum[i] != cmp.mNum[i])
            return false;
    }

    for (; i < mWordCount; i++){ 
        if (mNum[i])
            return false;
    }
    for (; i < cmp.mWordCount; i++){
        if (cmp.mNum[i])
            return false;
    }
}

template<class T>
bool LNA<T>::operator>=(const LNA<T>& cmp) const
{
	size_t i;
	for (i = mWordCount - 1; i >= cmp.mWordCount; i--){
		if (mNum[i])
			return true; // strickly greater than. has higher, non zero, words.
	}

	for (size_t j = cmp.mWordCount - 1; j != (size_t)-1; j--){
		if (mNum[i] > cmp[j])
			return true; // strickly greater than
		if (mNum[i] < cmp[j])
			return false; //  strickly less than
	}
	return true; // equal
}

template<class T>
void LNA<T>::operator+=(const LNA<T>& op)
{
    add(*this, op);
}

template<class T>
void LNA<T>::operator*=(const LNA<T>& op)
{
	LNA<T> sum;
	multiply(*this, op, sum);

	delete mNum;
	mNum = sum.mNum;
	sum.mNum = nullptr;
}

template<class T>
LNA<T>& LNA<T>::operator=(const LNA<T>& rhs)
{
	if (mWordCount < rhs.mWordCount)
	{
		delete mNum;
		mNum = new T[rhs.mWordCount];
		mWordCount = rhs.mWordCount;
	}
	else if (mWordCount > rhs.mWordCount){
		memset(mNum + rhs.mWordCount, 0, mWordCount - rhs.mWordCount); // set high words to zero.
	}
	memcpy(mNum, rhs.mNum, rhs.mWordCount);
	this->retract();
}

template<class T>
std::ostream& operator << (std::ostream& stream, const  LNA<T>& num)
{
	int width = 2 * sizeof(T);
	
	for (size_t i = num.mWordCount - 1; i != (size_t)-1; i--){
		cout << setfill('0') << setw(width) << std::hex << num[i];
	}
	cout << "'";

	return stream;
}

