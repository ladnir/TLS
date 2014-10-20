#pragma once
#include <stdint.h>
#include <vector>

#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>
#include <random>
#include "stackTrace.h"

//#define ST


void myAssert(bool b)
{
    if (!b){
        assert(b);
    }
}

void myAssert(bool b, std::string message)
{
    
    if (!b){
        assert(b);
    }
}

template<class T>
class LNA
{
public:
    static bool show;
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

	void expand(size_t minSize);
	void retract();
    void clearResize(size_t size);
    void clear();
    bool isZero() const;
    void copy(const LNA<T>&);
    T& getWord(const size_t idx)const;

	T&      operator[]  (const size_t&)const;
    //bool    operator>=  (const LNA<T>&)const;
    bool    operator<=  (const LNA<T>&)const;
    bool    operator<  (const LNA<T>&)const;
    bool    operator==  (const LNA<T>&)const;
    LNA<T>& operator=   (const LNA<T>&);
    void    operator-=  (const LNA<T>&);
    void    operator+=  (const LNA<T>&);
    void    operator*=  (const LNA<T>&);
    void    operator<<= (const size_t& bits);
    void    operator>>= (const size_t& bits);

    void randomize(size_t size);

    size_t mWordCount;
	size_t mWordSize;

    T* mNum;
};


#include "BitIterator.h"

template<class T>
bool LNA<T>::show = false;

template<class T>
LNA<T>::LNA(const std::vector<T>& source)
{
#ifdef ST    
    string s("LNA");
    stackTrace st(s); 
#endif
	mWordSize = sizeof(T) * 8;

    size_t pow2 = 1;
    while (pow2 < source.size()){
        pow2 <<= 1;
    }

    mWordCount = pow2;
	mNum = new T[mWordCount]();

	for (size_t i = 0; i < source.size(); i++){
        getWord(i) = source[source.size() - i - 1];
	}

}

template<class T>
LNA<T>::LNA(const LNA<T>& src)
{
#ifdef ST    
    string s("LNA");
   stackTrace st(s); 
   #endif
	mNum = new T[src.mWordCount];
	mWordCount = src.mWordCount;
	mWordSize = src.mWordSize;

	memcpy(mNum, src.mNum, src.mWordCount * sizeof(T));
}
template<class T>
LNA<T>::LNA()
{
#ifdef ST    
    string s("LNA");
    stackTrace st(s);
#endif
	mWordSize = sizeof(T) * 8;
	mWordCount = 1;
	mNum = new T[mWordCount]();
}

template<class T>
LNA<T>::~LNA()
{
#ifdef ST    
    string s("~LNA");
    stackTrace st(s);
#endif
    if (mNum != nullptr){
    	delete mNum;
        mNum = nullptr;
    }
}



template<class T>
void LNA<T>::expand(size_t size)
{
#ifdef ST    
    string s("expand");
    stackTrace st(s);
#endif
	myAssert(size > mWordCount);

    size_t pow2 = 1;
    while (pow2 < size){
        pow2 <<= 1;
    }

	T* temp = mNum;
    mNum = new T[pow2](); // zero initialized

	memcpy(mNum , temp, mWordCount * sizeof(T)); // copy old value;

	delete temp;
    mWordCount = pow2;
}

template<class T>
void LNA<T>::retract()
{

	size_t newSize = mWordCount;
    size_t i = mWordCount - 1;
    size_t halfNewSize;

    // lets see if we can make it smaller; half, a quarter, an eigth, a sixteenth, ...
    while (i){

         halfNewSize = newSize >> 2;
        for (;i && i >= halfNewSize; i --){
            if (getWord(i) != 0)
            {
                // We found a non zero word, lets see if we can retact
                if (newSize != mWordCount){
                    T* temp = mNum;
                    mNum = new T[newSize];
                    memcpy(mNum, temp, newSize * sizeof(T));

                    delete temp;
                    mWordCount = newSize;
                }
                return;
            }
        }
        newSize = halfNewSize;
    }

    // special case, all zero array of size > 1
    if (i == 0 && newSize != mWordCount){
        delete mNum;
        mNum = new T[1]();
        mWordCount = 1;
    }   
}

template<class T>
void LNA<T>::clearResize(size_t size)
{
    size_t pow2 = 1;
    while (pow2 < size){
        pow2 <<= 1;
    }

    delete mNum;
    mNum = new T[pow2]();
    mWordCount = pow2;
}

template<class T>
void LNA<T>::clear()
{

	memset(mNum, 0,mWordCount * sizeof(T));
}

template<class T>
bool LNA<T>::isZero() const
{

    for (size_t i = 0; i < mWordCount; i++){
        if (getWord(i)) return false;
    }
    return true;
}

template<class T>
void LNA<T>::copy(const LNA<T>& src)
{

    if (mNum != nullptr){
        delete mNum;
    }

    mNum = new T[src.mWordCount];
    mWordCount = src.mWordCount;

    memcpy(mNum, src.mNum, mWordCount * sizeof(T));
}

template<class T>
void LNA<T>::multiply(const LNA<T>& multiplicand, 
				      const LNA<T>& multiplier, 
					        LNA<T>& product)
{

    //std::cout << "multiplicand " << multiplicand << std::endl;
    //std::cout << "multiplier   " << multiplier << std::endl;
    product.clear();
	LNA<T> multShift(multiplier);
	BitIterator<T> multIter(multiplicand);

	while (multIter.isInRnage()){
        //std::cout << multIter << endl;

		if (*multIter){
			product += multShift;
        }
		multShift <<= 1;
		++multIter;
    }
}

template<class T>
std::string spacing(const LNA<T>& printed, const LNA<T> cmp)
{
    std::string ret;
    for (int i = printed.mWordCount; i < cmp.mWordCount; i++ )
        ret += "    ";

    return ret;
}

template<class T>
void LNA<T>::division(const LNA<T>& dividend,
					  const LNA<T>& divisor,
					        LNA<T>& quotient,
					        LNA<T>& remainder)
{

    myAssert(!divisor.isZero(), "Divide by zero error.");
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

    int shifts = dividend.mWordCount * dividend.mWordSize
                  - divisor.mWordCount  * divisor.mWordSize
                  - dividendLeadingZeros 
                  + divisorLeadingZeros;

    if (shifts < 0){
        return;
    }

    size_t reqSize = (shifts + (quotient.mWordSize)) / quotient.mWordSize;
    quotient.clearResize(reqSize);

    quotientIter.goToBit(shifts);
    divisorShift <<= shifts;

    if (1){
        cout << "" << endl;
    }
    //cout << "divisor  " << divisor << endl;
    //cout << "divshift "<< divisorShift << endl;

    while (shifts != (size_t)-1){
        if (divisorShift <= remainder){

            if (show){
                cout << "  " << spacing(remainder, divisorShift) <<  remainder << endl;
                cout << " -" << divisorShift << endl;
                cout << "_______________________________________" << endl;
            }
            remainder -= divisorShift;
            if (show){
                cout << "  " << remainder << endl << endl;
            }
            quotientIter.flipBit();
        }

        --quotientIter;
        divisorShift>>=1;
        shifts--;
    }
    cout << "";
}

template<class T>
void LNA<T>::add(LNA<T>& target,const LNA<T>& source)
{

	if (target.mWordCount < source.mWordCount){
		target.expand(source.mWordCount);
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
			target.expand(target.mWordCount + 1);
			target[target.mWordCount - 1]++;
		}
		else{
			// keep applying the carry to target
			for (size_t i = source.mWordCount;i < target.mWordCount; i++){
				target[i]++;
				if (target[i]) // if its non zero, we can stop since there are no more carries
					return; // all done
			}

			// Need to expand to fit the last carry
			if (target[target.mWordCount - 1] == 0){
				target.expand(target.mWordCount + 1);
				target[target.mWordCount - 1] ++;
			}
		}
	}
}


template<class T>
void LNA<T>::subtract(LNA<T>& target, const LNA<T>& source)
{

	T t1;
    size_t idx = source.mWordCount - 1;
    for (;idx >= target.mWordCount; idx--){
        myAssert(source[idx] == 0);//&& "NOT IMPLEMENTED: subtraction resulted in a negative number"); 
    }

	for (; idx != (size_t) -1; idx--){
		t1 = target[idx];
		target[idx] -= source[idx];

		if (target[idx] > t1){
			// borrow from above
			size_t i;
			for (i = idx + 1; 1; i++){
                myAssert(i < target.mWordCount);//&& "NOT IMPLEMENTED: subtraction resulted in a negative number"); // assert i >= 0

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
T& LNA<T>::getWord(const size_t idx) const
{

    if (idx >= mWordCount)
    {
        myAssert(0);
    }
    return mNum[idx];
}

template<class T>
T& LNA<T>::operator[](const uint32_t& idx) const
{

    if (idx >= mWordCount)
    {
        myAssert(0);
    }
    return mNum[idx];
}

template<class T>
void LNA<T>::operator<<=(const size_t& shifts)
{

    //assert(shifts < mWordCount * mWordSize);

    size_t wordShifts = shifts / mWordSize;
    size_t bitShifts = shifts % mWordSize;
    size_t wordsAdded = (shifts + (mWordSize - 1)) / mWordSize;

    // decrease the number of words we need to add if the 
    // array already has extra room to shift the bits over into.
    size_t i = mWordCount - 1;
    for (; 
            getWord(i) == 0 && 
            wordsAdded && 
            i != (T)-1; 
            i--)
    {
        wordsAdded--;
    }

    // 
    if (wordsAdded && bitShifts && getWord(i) >> (mWordSize - bitShifts) == 0)
        wordsAdded--;

    //if (show) 
    //    cout << "b " << *this << endl;

    if (wordsAdded) 
        expand(mWordCount + wordsAdded);

    //if (show) 
    //    cout << "a " << *this << endl;

    size_t upperIdx = mWordCount - 1;
    size_t lowwerIdx = upperIdx - wordShifts;

    while (lowwerIdx != 0){
        T upperbits = (getWord(lowwerIdx) << bitShifts);
        T lowwerBits = (getWord(lowwerIdx - 1) >> (mWordSize - bitShifts));
        getWord(upperIdx--) = upperbits + lowwerBits;
                           
        lowwerIdx--;
    }

    getWord(upperIdx--) = getWord(0) << bitShifts;

    while (upperIdx != (size_t)-1){
        getWord(upperIdx--) = 0;
    }
}

template<class T>
void LNA<T>::operator>>=(const size_t& shifts)
{

    myAssert(shifts < mWordCount * mWordSize);

    size_t wordShifts = shifts / mWordSize;
    size_t bitShifts = shifts % mWordSize;

    size_t upperIdx = wordShifts,
           lowwerIdx = 0;

    while (upperIdx != mWordCount - 1){
        T upperbits =  getWord(lowwerIdx) >> bitShifts ;
        T lowwerBits = getWord(lowwerIdx + 1)   << (sizeof(T)* 8 - bitShifts);
        getWord(upperIdx++) = upperbits + lowwerBits;
        lowwerIdx++;    
    }

    getWord(upperIdx++) = getWord(mWordCount - 1) >> bitShifts;

    while (lowwerIdx != mWordCount - 1){
        getWord(upperIdx++) = 0;
    }

    retract();
}


template<class T>
bool LNA<T>::operator==(const LNA<T>& cmp) const
{
    size_t min = (mWordCount > cmp.mWordCount)? cmp.mWordCount : mWordCount;
    size_t i;

    for (i = 0; i < min; i++){
        if (getWord(i) != cmp.getWord(i))
            return false;
    }

    for (; i < mWordCount; i++){ 
        if (getWord(i))
            return false;
    }
    for (; i < cmp.mWordCount; i++){
        if (cmp.getWord(i))
            return false;
    }
}

//template<class T>
//bool LNA<T>::operator>=(const LNA<T>& cmp) const
//{

//}

template<class T>
bool LNA<T>::operator<=(const LNA<T>& cmp) const
{

    size_t i;

    if (mWordCount > cmp.mWordCount){
        for (i = mWordCount - 1; i >= cmp.mWordCount; i--){
            if (getWord(i))
                return false; // strickly greater than. has higher, non zero, words.
        }
    }
    else{
        for (i = cmp.mWordCount - 1; i >= mWordCount; i--){
            if (cmp.getWord(i))
                return true; // strickly less than. cmp has higher, non zero, words.
        }
    }
    //cout << "is " << *this << endl;
    //cout << "<= " << cmp << endl << endl;

    int width = 2 * sizeof(T);
    for (; i != (size_t)-1; i--){
        //cout << "mNum " << setfill('0') << setw(width) << std::hex << getWord(i] << endl;
        //cout << "cmp  " << setfill('0') << setw(width) << std::hex << cmp[i] << endl;
        if (getWord(i) > cmp[i])
            return false; // strickly greater than
        if (getWord(i) < cmp[i])
            return true; //  strickly less than
    }
    return true; // equal
}
template<class T>
bool LNA<T>::operator<(const LNA<T>& cmp) const
{
#ifdef ST    
    string s(":operator<");
    stackTrace st(s);
#endif
    size_t i;

    if (mWordCount > cmp.mWordCount){
        for (i = mWordCount - 1; i >= cmp.mWordCount; i--){
            if (getWord(i))
                return false; // strickly greater than. has higher, non zero, words.
        }
    }
    else{
        for (i = cmp.mWordCount - 1; i >= mWordCount; i--){
            if (cmp.getWord(i))
                return true; // strickly less than. cmp has higher, non zero, words.
        }
    }
    //cout << "is " << *this << endl;
    //cout << "<= " << cmp << endl << endl;

    int width = 2 * sizeof(T);
    for (; i != (size_t)-1; i--){
        //cout << "mNum " << setfill('0') << setw(width) << std::hex << getWord(i] << endl;
        //cout << "cmp  " << setfill('0') << setw(width) << std::hex << cmp[i] << endl;
        if (getWord(i) > cmp[i])
            return false; // strickly greater than
        if (getWord(i) < cmp[i])
            return true; //  strickly less than
    }
    return false; // equal
}

template<class T>
void LNA<T>::operator+=(const LNA<T>& op)
{
#ifdef ST   
    string s(":operator+=");
    stackTrace st(s);
#endif
    add(*this, op);
}

template<class T>
void LNA<T>::operator-=(const LNA<T>& op)
{
#ifdef ST    
    string s("operator-=");
    stackTrace st(s);
#endif
    subtract(*this, op);
}

template<class T>
void LNA<T>::operator*=(const LNA<T>& op)
{

	LNA<T> sum;
	multiply(*this, op, sum);

	delete mNum;
	mNum = sum.mNum;
	sum.mNum = nullptr;
    mWordCount = sum.mWordCount;
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
		memset(mNum + rhs.mWordCount, 0, (mWordCount - rhs.mWordCount) * sizeof(T)); // set high words to zero.
	}
	memcpy(mNum, rhs.mNum, rhs.mWordCount * sizeof(T));
	this->retract();
}

template<class T>
void LNA<T>::randomize(size_t size)
{

    clearResize(size);

    static std::default_random_engine generator;
    static std::uniform_int_distribution<T> distribution(0, (T)-1);
    for (size_t i = 0; i < mWordCount; i++)
        getWord(i) = distribution(generator);

}

template<class T>
std::ostream& operator<< (std::ostream& stream, const  LNA<T>& num)
{

	int width = 2 * sizeof(T);
	
	for (size_t i = num.mWordCount - 1; i != (size_t)-1; i--){
		cout << setfill('0') << setw(width) << std::hex << num[i];
	}
	cout << "'";

	return stream;
}

