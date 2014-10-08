#pragma once
#include <stdint.h>
#include <vector>

template<class T>
class LNA
{
public:
    LNA(const std::vector<T>&);
    LNA();
    ~LNA();

    static void add(LNA<T>& target,const LNA<T>& source);
	static void subtract(LNA<T>& target, const LNA<T>& source);
	static void multiply(LNA<T>& target, const LNA<T>& source);

	void resize(size_t);
	bool retract();

	T& operator[](const size_t&) const;
	bool operator>=(const LNA<T>&)const;
	LNA<T>& operator=(const LNA<T>&);
	LNA<T>& operator<<(int size_t& bits);
    size_t mWordCount;
	size_t mWordSize;

    T* mNum;
};

#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>


template<class T>
LNA<T>::LNA(const std::vector<T>& source)
{
	mWordSize = sizeof(T);
	mWordCount = source.size();
	mNum = new T[mWordCount];

	for (size_t i = 0; i < mWordCount; i++){
		mNum[i] = source[mWordCount - i - 1];
	}

}

template<class T>
LNA<T>::LNA()
{
	mWordSize = sizeof(T);
	mWordCount = 1;
	mNum = new T[mWordCount];
}

template<class T>
LNA<T>::~LNA()
{
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
static void multiply(LNA<T>& target, const LNA<T>& source)
{
	BitIterator<T> srcIter(source);

	
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
LNA<T>& LNA<T>::operator<<(const size_t& shifts)
{
	
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

