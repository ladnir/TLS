#pragma once
#include <stdint.h>
#include <vector>

template<class T>
class LNA
{
public:
    LNA(std::vector<T>&);
    LNA();
    ~LNA();

    static void add(LNA<T>& target,const LNA<T>& source);
	static void subtract(LNA<T>& target, const LNA<T>& source);
	static void multiply(LNA<T>& target, const LNA<T>& source);

    void resize(uint32_t);
	bool retract();

	T& operator[](const uint32_t&) const;
	bool operator>=(const LNA<T>&)const;

    size_t mWorkCount;
	size_t mWordSize;

    T* mNum;
};

#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>


template<class T>
LNA<T>::LNA(std::vector<T>& source)
{
	mWordSize = sizeof(T);
	mWorkCount = source.size();
	mNum = new T[mWorkCount];

	memcpy(mNum, &source[0], mWorkCount * sizeof(T));
}

template<class T>
LNA<T>::LNA()
{
	mWordSize = sizeof(T);
	mWorkCount = 1;
	mNum = new T[mWorkCount];
}

template<class T>
LNA<T>::~LNA()
{
	delete mNum;
}

template<class T>
void LNA<T>::resize(uint32_t size)
{
	assert(size > mWorkCount);

	T* temp = mNum;
	mNum = new T[size](); // zero initialized

	memcpy(mNum + (size - mWorkCount), temp, mWorkCount * sizeof(T)); // copy old value;

	delete temp;
	mWorkCount = size;
}

template<class T>
bool LNA<T>::retract()
{
	size_t half = mWorkCount / 2;
	for (int i = 0; i < half; i++){
		if (mNum[i])
			return false; // atleast half full
	}

	T* temp = mNum;
	mNum = new T[half];
	for (int i = 0; i < half; i++){
		mNum[i] = temp[half + i];
	}
	delete temp; 
	mWorkCount = half;
	return true;
}

template<class T>
void LNA<T>::add(LNA<T>& target,const LNA<T>& source)
{
	if (target.mWorkCount < source.mWorkCount){
		target.resize(source.mWorkCount);
	}

	T carry = 0;
	T t1;
	
	for (int tIdx = target.mWorkCount - 1,
			 sIdx = source.mWorkCount - 1;
		sIdx >= 0;
		sIdx--, tIdx--)
	{
		if (source[sIdx] || carry){
			t1 = target[tIdx];
			target[tIdx] += source[sIdx] + carry;
			//cout << target << endl;
			carry = (target[tIdx] <= t1) ? 1 : 0;
		}
	}

	if (carry){

		if (target.mWorkCount == source.mWorkCount){
			// expand target and set its top work to 1
			target.resize(target.mWorkCount + 1);
			target[0]++;
		}
		else{
			// keep applying the carry to target
			for (int32_t i = target.mWorkCount - source.mWorkCount -1;i >= 0; i--){
				target[i]++;
				if (target[i]) // if its non zero, we can stop since there are no more carries
					return; // all done
			}

			// Need to resize to fit the last carry
			if (target[0] == 0){
				target.resize(target.mWorkCount + 1);
				target[0] ++;
			}
		}
	}
}


template<class T>
void LNA<T>::subtract(LNA<T>& target, const LNA<T>& source)
{
	//assert(target >= source && "cant be negative");

	size_t tIdx = target.mWorkCount - source.mWorkCount;
	T t1;

	for (size_t sIdx = 0; sIdx < source.mWorkCount; sIdx++, tIdx++){
		t1 = target[tIdx];
		target[tIdx] -= source[sIdx];

		if (target[tIdx] > t1){
			// borrow from above
			size_t i;
			for (i = tIdx - 1; 1; i--){
				assert(i != (size_t)-1 && "NOT IMPLEMENTED: subtraction resulted in a negative number"); // assert i >= 0

				if (target[i]){
					target[i] --; // found somewhere to borrow from.

					for (; i > tIdx - 1;i--){ // set all intermidiate values to the max value
						target[i] = (T)-1; //  target[i] = 9999...
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
	assert(idx < mWorkCount);
	return mNum[idx];
}

template<class T>
bool LNA<T>::operator>=(const LNA<T>& cmp) const
{
	int i;
	for (i = 0; i < mWorkCount - cmp.mWorkCount; i++){
		if (mNum[i])
			return true; // strickly greater than. has higher, non zero, words.
	}

	for (int j = 0; j < cmp.mWorkCount; j++, i++){
		if (mNum[i] > cmp[j])
			return true; // strickly greater than
		if (mNum[i] < cmp[j])
			return false; // strickly less than
	}
	return true; // equal
}
template<class T>
std::ostream& operator << (std::ostream& stream, const  LNA<T>& num)
{
	int width = 2 * sizeof(T);
	
	for (uint32_t i = 0; i < num.mWorkCount; i++){
		cout << setfill('0') << setw(width) << std::hex << num[i];
	}
	cout << "'";

	return stream;
}