#include "LNA.h"
#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>

template<class T>
LNA<T>::LNA(std::vector<T>& source)
{
    mWorkCount = source.size();
    mNum = new T[mWorkCount];

    memcpy(mNum, &source[0], mWorkCount * sizeof(T));
}

template<class T>
LNA<T>::LNA()
{
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
void LNA<T>::add(LNA<T>& target, LNA<T>& source)
{
    if (target.mWorkCount < source.mWorkCount){
        target.resize(source.mWorkCount);
    }

    T carry = 0;
    T temp;

    for (uint32_t i = 0; i < source.mWorkCount; i++){
        temp = target.mNum[i];
        target.mNum[i] += source.mNum[i] + carry;
        carry = (target.mNum[i] < temp )? 1 : 0;
    }

    if (carry){

        if (target.mWorkCount == source.mWorkCount){
            // expand target and set its top work to 1
            target.resize(target.mWorkCount + 1);
            target.mNum[target.mWorkCount - 1]++;
        }
        else{
            // keep applying the carry to target
            for (uint32_t i = source.mWorkCount; i < target.mWorkCount; i++){
                target.mNum[i]++;
                if (target.mNum[i]) // if its non zero, we can stop since there are no more carries
                    return; // all done
            }

            // Need to resize to fit the last carry
            if (target.mNum[target.mWorkCount - 1] == 0){
                target.resize(target.mWorkCount + 1);
                target.mNum[target.mWorkCount - 1] ++;
            }
        }
    }
}



template<class T>
std::ostream& operator << (std::ostream& stream, const  LNA<T>& num)
{
    std::ios  state(NULL);
    state.copyfmt(stream);

    stream << setfill('0') << setw(4 * sizeof(T)) << std::hex; // output hex

    for (uint32_t i = 0; i < num.mWorkCount; i++){
        stream << num.mNum[i];
    }
    stream << "'";

    std::cout.copyfmt(state); // restore the streams modifiers

    return stream;
}