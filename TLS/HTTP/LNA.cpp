#include "BitIterator.h"
#include "LNA.h"
#include <algorithm>

#include "LNAShifts.h"

//
//void myAssert(bool b)
//{
//    if (!b){
//        assert(b);
//    }
//}
//
//void myAssert(bool b, std::string message)
//{
//
//    if (!b){
//        assert(b);
//    }
//}

using namespace std;

bool LNA::show = false;
const LNA LNA::one = LNA(1);


LNA::LNA(const T num){
    mWordSize = sizeof(T)* 8;
    mWordCount = 1;
    mNum = new T[mWordCount]();

    getWord(0) = num;
}

//template<class G>
LNA::LNA(WordOrder wo, const vector<uint8_t>& source )
{
    vectorInit(wo, source.begin(), source.end());
}

//template<class R>
LNA::LNA(WordOrder                  wo, 
         vector<uint8_t>::const_iterator& start, 
         vector<uint8_t>::const_iterator& end)
{
    vectorInit(wo, start, end);
}


LNA::LNA(const LNA& src)
{
    mNum = new T[src.mWordCount];
    mWordCount = src.mWordCount;
    mWordSize = src.mWordSize;

    memcpy(mNum, src.mNum, src.mWordCount * sizeof(T));
}

LNA::LNA()
{
    mWordSize = sizeof(T)* 8;
    mWordCount = 1;
    mNum = new T[mWordCount]();
}


LNA::~LNA()
{
    if (mNum != nullptr){
        delete mNum;
        mNum = nullptr;
    }
}

//template<class R>
void LNA::vectorInit(WordOrder                  wo, 
                     vector<uint8_t>::const_iterator& start, 
                     vector<uint8_t>::const_iterator& end)
{
    if (mNum != nullptr)
        delete mNum;

    mWordSize = sizeof(T)* 8;

    size_t sourceByteLength = ( end - start) * sizeof(uint8_t);

    size_t pow2 = 1;
    while (pow2 < sourceByteLength){
        assert(pow2);
        pow2 <<= 1;
    }

    mWordCount = (pow2 + sizeof(T) - 1) / sizeof(T);
    mNum = new T[mWordCount]();

    if (wo == WordOrder::LSWF){
        memcpy(mNum, &(*start), sourceByteLength);
    }
    else{
        vector<uint8_t> sourceReversed(start, end);
        std::reverse(sourceReversed.begin(), sourceReversed.end());

        memcpy(mNum, &sourceReversed[0], sourceByteLength);
    }

}

void LNA::expand(size_t size)
{
    assert(size > mWordCount);

    size_t pow2 = 1;
    while (pow2 < size){
        pow2 <<= 1;
    }

    T* temp = mNum;
    mNum = new T[pow2](); // zero initialized

    memcpy(mNum, temp, mWordCount * sizeof(T)); // copy old value;

    delete temp;
    mWordCount = pow2;
}


void LNA::retract()
{

    size_t newSize = mWordCount;
    size_t i = mWordCount - 1;
    size_t halfNewSize;

    // lets see if we can make it smaller; half, a quarter, an eigth, a sixteenth, ...
    while (i != (size_t)-1){

        halfNewSize = newSize >> 1;
        for (; i != (size_t)-1 && i >= halfNewSize; i--){
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
    delete mNum;
    mNum = new T[1]();
    mWordCount = 1;
}


void LNA::clearResize(size_t size)
{
    size_t pow2 = 1;
    while (pow2 < size){
        pow2 <<= 1;
    }

    delete mNum;
    mNum = new T[pow2]();
    mWordCount = pow2;
}


void LNA::clear()
{

    memset(mNum, 0, mWordCount * sizeof(T));
}


bool LNA::isZero() const
{

    for (size_t i = 0; i < mWordCount; i++){
        if (getWord(i)) return false;
    }
    return true;
}


void LNA::copy(const LNA& src)
{

    if (mNum != nullptr){
        delete mNum;
    }

    mNum = new T[src.mWordCount];
    mWordCount = src.mWordCount;

    memcpy(mNum, src.mNum, mWordCount * sizeof(T));
}


void LNA::multiply(const LNA& multiplicand,
                   const LNA& multiplier,
                         LNA& product)
{
    product.clear();
    LNA multShift(multiplier);
    BitIterator<T> multIter(multiplicand);

    while (multIter.isInRnage()){
        if (*multIter){
            product += multShift;
        }
        multShift <<= 1;
        ++multIter;
    }
}

void LNA::multiply(const LNA& multiplicand,
                   const LNA& multiplier,
                   const LNA& modulus,
                         LNA& product)
{
    product.clear();
    LNA multShift(multiplier);
    BitIterator<T> multIter(multiplicand);

    while (multIter.isInRnage()){
        if (*multIter){
            add(product, multShift);//, modulus);
        }
        multShift <<= 1;
        ++multIter;
    }

    //if (modulus < product)
    //    product %= modulus;
}

void LNA::secureExponentiate(const LNA& base, 
                             const LNA& exponent, 
                             const LNA& modulus, 
                                   LNA& remainder)
{
    BitIterator<T> exponIter(exponent);
    LNA squares(base);
    LNA temp2;
    LNA temp3;

    remainder = LNA::one;
    
    while (exponIter.isInRnage()){
        if (*exponIter){
            // remainder = (remainder * squares) % modulus

            //temp3 = remainder;
            // multiply(temp3, squares, modulus, remainder);

            remainder *= squares;
            remainder %= modulus;
        }
        else{
            temp3 = remainder; 
            multiply(temp3, squares, modulus, temp2);
            temp2 %= modulus;
        }

        // squares = (squares * squares) % modulus
        
        temp2 = squares;

        squares *= temp2;
        squares %= modulus;
        //multiply(temp2, temp2, modulus, squares);

        ++exponIter;
    }

}

void LNA::exponentiate(const LNA& base, 
                       const LNA& exponent, 
                       const LNA& modulus, 
                             LNA& remainder)
{
    BitIterator<T> exponIter(exponent);
    LNA squares(base);
    LNA temp2;

    remainder = LNA::one;

    while (exponIter.isInRnage()){
        if (*exponIter){
            remainder *= squares;
            remainder %= modulus;
        }

        // square
        temp2 = squares;
        squares *= temp2;
        squares %= modulus;

        ++exponIter;
    }
}

void LNA::exponentiate(LNA& base,
                       const LNA& exponent)
{
    BitIterator<T> exponIter(exponent);
    LNA squares(base);
    LNA temp2;

    base == LNA::one;

    while (exponIter.isInRnage()){
        if (*exponIter){
            // multiply
            base *= squares;
        }

        // square
        temp2 = squares;
        squares *= temp2;

        ++exponIter;
    }
}

void LNA::mod( LNA& dividendRemainder,
               const LNA& divisor)
{

    assert(!divisor.isZero() && "Divide by zero error.");

    if (dividendRemainder.isZero()){
        return;
    }

    LNA divisorShift(divisor);

    BitIterator<T> divisorIter(divisor);
    BitIterator<T> dividentIter(dividendRemainder);

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

    int shifts = static_cast<int>(dividendRemainder.mWordCount * dividendRemainder.mWordSize
        - divisor.mWordCount  * divisor.mWordSize
        - dividendLeadingZeros
        + divisorLeadingZeros);

    if (shifts < 0){
        return;
    }

    //LNAShifts divisorShift(divisor, shifts, "<<");

    divisorShift <<= shifts;


    while (shifts != (size_t)-1){
        
        if (divisorShift <= dividendRemainder){
            dividendRemainder -= divisorShift;

        }
        divisorShift >>= 1;
        shifts--;
        //divisorShift.goToShift(shifts);
    }
}


void LNA::division(const LNA& dividend,
                   const LNA& divisor,
                   LNA& quotient,
                   LNA& remainder)
{

    assert(!divisor.isZero() && "Divide by zero error.");
    quotient.clear();
    if (dividend.isZero()){
        remainder.clear();
        return;
    }

    LNA divisorShift(divisor);
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

    int shifts = static_cast<int>(dividend.mWordCount * dividend.mWordSize
                                - divisor.mWordCount  * divisor.mWordSize
                                - dividendLeadingZeros
                                + divisorLeadingZeros);

    if (shifts < 0){
        return;
    }

    size_t reqSize = (shifts + (quotient.mWordSize)) / quotient.mWordSize;
    quotient.clearResize(reqSize);

    quotientIter.goToBit(shifts);
    divisorShift <<= shifts;


    //LNAShifts divisorShift(divisor, shifts, "<<");

    while (shifts != (size_t)-1){
        if (divisorShift <= remainder){
            remainder -= divisorShift;
            quotientIter.flipBit();
        }

        --quotientIter;
        divisorShift >>= 1;
        shifts--;
        //divisorShift.goToShift(shifts);
    }
}

void LNA::add(LNA& target, const LNA& source)
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
            size_t carryIdx = target.mWordCount;
            target.expand(target.mWordCount + 1);
            target[carryIdx]++;
        }
        else{
            // keep applying the carry to target
            for (size_t i = source.mWordCount; i < target.mWordCount; i++){
                target[i]++;
                if (target[i]) // if its non zero, we can stop since there are no more carries
                    return; // all done
            }

            // Need to expand to fit the last carry
            if (target[target.mWordCount - 1] == 0){
                size_t carryIdx = target.mWordCount;
                target.expand(target.mWordCount + 1);
                target[carryIdx] ++;
            }
        }
    }
}



void LNA::subtract(LNA& target, const LNA& source)
{

    T t1;
    size_t idx = source.mWordCount - 1;
    for (; idx >= target.mWordCount; idx--){
        assert(source[idx] == 0);//&& "NOT IMPLEMENTED: subtraction resulted in a negative number"); 
    }

    for (; idx != (size_t)-1; idx--){
        t1 = target[idx];
        target[idx] -= source[idx];

        if (target[idx] > t1){
            // borrow from above
            size_t i;
            for (i = idx + 1; 1; i++){
                assert(i < target.mWordCount);//&& "NOT IMPLEMENTED: subtraction resulted in a negative number"); // assert i >= 0

                if (target[i]){
                    target[i] --; // found somewhere to borrow from.

                    for (i--; i > idx; i--){ // set all intermidiate values to the max value
                        target[i] = (T)-1; //  target[i] = ffff...
                    }
                    break;
                }
            }
        }
    }
    target.retract();
}

size_t LNA::byteLength() const
{
    return mWordCount * sizeof(T);
}

LNA::T& LNA::getWord(const size_t idx) const
{
    assert(idx < mWordCount);
    return mNum[idx];
}


LNA::T& LNA::operator[](const size_t& idx) const
{
    assert(idx < mWordCount);
    return mNum[idx];
}


void LNA::operator<<=(const size_t& shifts)
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

    if (wordsAdded)
        expand(mWordCount + wordsAdded);

    size_t upperIdx = mWordCount - 1;
    size_t lowwerIdx = upperIdx - wordShifts;

    int width = 2 * sizeof(T);

    if (bitShifts){
        while (lowwerIdx != 0){
            T upperbits = (getWord(lowwerIdx) << bitShifts);
            T lowwerBits = (getWord(lowwerIdx - 1) >> (mWordSize - bitShifts));
            getWord(upperIdx--) = upperbits + lowwerBits;

            lowwerIdx--;
        }
        getWord(upperIdx--) = getWord(0) << bitShifts;


    }
    else{
        //// bit shifting a word  >> by its width is a no op. This happens
        //// when bitShift == 0. This is a fix for that special case.
        //T* dest = mNum + wordShifts * sizeof(T);
        //T* src = mNum;

        while (lowwerIdx != (size_t)-1){
            getWord(upperIdx--) = getWord(lowwerIdx--);
        }

    }
    while (upperIdx != (size_t)-1){
        getWord(upperIdx--) = 0;
    }
}


void LNA::operator>>=(const size_t& shifts)
{
    assert(shifts < mWordCount * mWordSize);

    size_t wordShifts = shifts / mWordSize;
    size_t bitShifts = shifts % mWordSize;

    size_t upperIdx = wordShifts,
        lowwerIdx = 0;

    if (bitShifts){

        while (upperIdx != mWordCount - 1){
            T upperbits = getWord(lowwerIdx) >> bitShifts;
            T lowwerBits = getWord(lowwerIdx + 1) << (sizeof(T)* 8 - bitShifts);
            getWord(upperIdx++) = upperbits + lowwerBits;
            lowwerIdx++;
        }
    }
    else{
        while (upperIdx != mWordCount - 1){

            getWord(upperIdx++) = getWord(lowwerIdx);
            lowwerIdx++;
        }
    }

    getWord(upperIdx++) = getWord(mWordCount - 1) >> bitShifts;

    while (lowwerIdx != mWordCount - 1){
        getWord(upperIdx++) = 0;
    }

    retract();
}



bool LNA::operator==(const LNA& cmp) const
{
    size_t min = (mWordCount > cmp.mWordCount) ? cmp.mWordCount : mWordCount;
    size_t i;

    for (i = 0; i < min; i++){
        if (getWord(i) != cmp.getWord(i)){
            return false;
        }
    }

    for (; i < mWordCount; i++){
        if (getWord(i)){
            return false;
        }
    }
    for (; i < cmp.mWordCount; i++){
        if (cmp.getWord(i)){
            return false;
        }
    }

    return true;
}

bool LNA::operator<=(const LNA& cmp) const
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

    int width = 2 * sizeof(T);
    for (; i != (size_t)-1; i--){
        if (getWord(i) > cmp[i])
            return false; // strickly greater than
        if (getWord(i) < cmp[i])
            return true; //  strickly less than
    }
    return true; // equal
}

bool LNA::operator<(const LNA& cmp) const
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

    int width = 2 * sizeof(T);
    for (; i != (size_t)-1; i--){
        if (getWord(i) > cmp[i])
            return false; // strickly greater than
        if (getWord(i) < cmp[i])
            return true; //  strickly less than
    }
    return false; // equal
}


void LNA::operator+=(const LNA& op)
{
    add(*this, op);
}


void LNA::operator-=(const LNA& op)
{

    subtract(*this, op);
}


void LNA::operator*=(const LNA& op)
{

    LNA sum;
    multiply(*this, op, sum);

    delete mNum;
    mNum = sum.mNum;
    sum.mNum = nullptr;
    mWordCount = sum.mWordCount;
}

void LNA::randomize(size_t size)
{

    clearResize(size);

    //static std::default_random_engine generator;
    //static std::uniform_int_distribution<T> distribution(0, (T)-1);
    for (size_t i = 0; i < mWordCount; i++){
        T num = rand();
        //T num = distribution(generator);
        //cout << num << endl;
        getWord(i) = num;
    }

}
void LNA::operator=(const LNA& rhs)
{
    copy(rhs);
}

LNA& LNA::operator++()
{
    add(*this,one);

    return *this;
}

void LNA::operator%=(const LNA& divisor){
    mod(*this, divisor);
}

void LNA::unload(WordOrder wo, size_t byteCount, vector<uint8_t>& dest)
{
    // check that the upper bytes are in fact 0 and that it is big enough
    uint8_t* byteView = (uint8_t*) mNum;

    for (size_t idx = mWordCount *sizeof(T) - 1;idx >= byteCount; idx--){
        assert(byteView[idx] == 0);
    }


    if (wo == WordOrder::MSWF){
        // LNA has a Least Significant Word First order, 

        vector<uint8_t> reverseNum;

        size_t idx = (byteCount + sizeof(T)-1) / sizeof(T) - 1;

        // if its smaller, fill the most significant words up with 0;
        for (; idx >= mWordCount; idx--){
            for (size_t subIdx = 0; subIdx < sizeof(LNA::T); subIdx++){
                reverseNum.push_back(0);
            }
        }
        
        for (; idx != (size_t)-1; idx--){
            for (size_t subIdx = sizeof(LNA::T) - 1; subIdx != (size_t)-1; subIdx--){

                reverseNum.push_back((uint8_t)(mNum[idx] >> (subIdx * 8)));
            }
        }
        //
        //for (idx *= sizeof(LNA::T); idx != (size_t)-1; idx--){
        //    reverseNum.push_back( byteView [idx]);
        //}


        size_t curSize = dest.size();

        if (curSize < curSize + byteCount)
            dest.resize(curSize + byteCount);

        dest[curSize + byteCount - 1] = 0; // force it to grow

        memcpy(&dest[curSize], &reverseNum[0], byteCount);
    }
    else{
        assert(0 && "not implemented");
    }
}

std::ostream& operator<< (std::ostream& stream, const  LNA& num)
{

    int width = 2 * sizeof(LNA::T);

    for (size_t i = num.mWordCount - 1; i != (size_t)-1; i--){
        if (sizeof(LNA::T) == 1){
            std::cout << std::setfill('0') << std::setw(width) << std::hex << (uint16_t) num[i];
        }
        else{
            std::cout << std::setfill('0') << std::setw(width) << std::hex << num[i];
        }
    }
    std::cout << "'";

    return stream;
}
