#include "BitIterator.h"
#include "LNA.h"



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

using namespace std;

bool LNA::show = false;


LNA::LNA(const std::vector<T>& source)
{
#ifdef ST    
    string s("LNA");
    stackTrace st(s);
#endif
    mWordSize = sizeof(T)* 8;

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


LNA::LNA(const LNA& src)
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

LNA::LNA()
{
#ifdef ST    
    string s("LNA");
    stackTrace st(s);
#endif
    mWordSize = sizeof(T)* 8;
    mWordCount = 1;
    mNum = new T[mWordCount]();
}


LNA::~LNA()
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




void LNA::expand(size_t size)
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


std::string spacing(const LNA& printed, const LNA cmp)
{
    std::string ret;
    for (size_t i = printed.mWordCount; i < cmp.mWordCount; i++)
        ret += "        ";

    return ret;
}


void LNA::multiply(const LNA& multiplicand,
    const LNA& multiplier,
    LNA& product)
{

    //std::cout << "multiplicand " << multiplicand << std::endl;
    //std::cout << "multiplier   " << multiplier << std::endl;
    product.clear();
    LNA multShift(multiplier);
    BitIterator<T> multIter(multiplicand);

    while (multIter.isInRnage()){
        //std::cout << multIter << endl;
        if (*multIter){
            if (show){
                cout << "  " << spacing(product, multShift) << product << endl;
                cout << " +" << multShift << endl;
                cout << "_______________________________________1" << endl;
            }
            product += multShift;
            if (show){
                cout << "  " << product << endl << endl;
            }
        }
        else if (show){
            cout << "                                       0" << endl;
        }
        multShift <<= 1;
        ++multIter;
    }
}


void LNA::division(const LNA& dividend,
    const LNA& divisor,
    LNA& quotient,
    LNA& remainder)
{

    myAssert(!divisor.isZero(), "Divide by zero error.");
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
    if (show) cout << "b " << divisorShift << endl;
    divisorShift <<= shifts;
    if (show) cout << "a " << divisorShift << endl;

    //if (1){
    //    cout << "" << endl;
    //}
    //cout << "divisor  " << divisor << endl;
    //cout << "divshift "<< divisorShift << endl;

    while (shifts != (size_t)-1){
        if (divisorShift <= remainder){

            if (show){
                //cout << "  " << spacing(remainder, divisorShift) << remainder << endl;
                //cout << " -" << divisorShift << endl;
                //cout << "_______________________________________1" << endl;
            }
            remainder -= divisorShift;
            if (show){
                //cout << "  " << remainder << endl << endl;
            }
            quotientIter.flipBit();
        }
        else if (show){

            //cout << " -" << divisorShift << endl;
            //cout << "                                       0" << endl;
        }
        //cout << quotientIter << endl;

        --quotientIter;
        divisorShift >>= 1;
        shifts--;
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
        myAssert(source[idx] == 0);//&& "NOT IMPLEMENTED: subtraction resulted in a negative number"); 
    }

    for (; idx != (size_t)-1; idx--){
        t1 = target[idx];
        target[idx] -= source[idx];

        if (target[idx] > t1){
            // borrow from above
            size_t i;
            for (i = idx + 1; 1; i++){
                myAssert(i < target.mWordCount);//&& "NOT IMPLEMENTED: subtraction resulted in a negative number"); // assert i >= 0

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

LNA::T& LNA::getWord(const size_t idx) const
{

    if (idx >= mWordCount)
    {
        myAssert(0);
    }
    return mNum[idx];
}


LNA::T& LNA::operator[](const size_t& idx) const
{

    if (idx >= mWordCount)
    {
        myAssert(0);
    }
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

    //if (show) 
    //    cout << "b " << *this << endl;

    if (wordsAdded)
        expand(mWordCount + wordsAdded);

    //if (show) 
    //    cout << "a " << *this << endl;

    size_t upperIdx = mWordCount - 1;
    size_t lowwerIdx = upperIdx - wordShifts;

    int width = 2 * sizeof(T);

    if (bitShifts){
        while (lowwerIdx != 0){
            //if (show){
            //    cout << "wu " << setfill('0') << setw(width) << std::hex << getWord(lowwerIdx) << endl;
            //    cout << "u  " << setfill('0') << setw(width) << std::hex << (getWord(lowwerIdx) << bitShifts) << endl;
            //    cout << "wl " << setfill('0') << setw(width) << std::hex << (getWord(lowwerIdx - 1)) << endl;
            //    cout << "l  " << setfill('0') << setw(width) << std::hex << (getWord(lowwerIdx - 1) >> (mWordSize - bitShifts)) << endl;
            //}
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

        //memmove(dest, src, (mWordCount - wordShifts) * sizeof(T)); // important to use memmove.

        //upperIdx = wordShifts - 1;
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

    myAssert(shifts < mWordCount * mWordSize);

    size_t wordShifts = shifts / mWordSize;
    size_t bitShifts = shifts % mWordSize;

    size_t upperIdx = wordShifts,
        lowwerIdx = 0;

    while (upperIdx != mWordCount - 1){
        T upperbits = getWord(lowwerIdx) >> bitShifts;
        T lowwerBits = getWord(lowwerIdx + 1) << (sizeof(T)* 8 - bitShifts);
        getWord(upperIdx++) = upperbits + lowwerBits;
        lowwerIdx++;
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
            cout << "fasle" << endl;
            return false;
        }
    }

    for (; i < mWordCount; i++){
        if (getWord(i)){
            cout << "fasle" << endl;
            return false;
        }
    }
    for (; i < cmp.mWordCount; i++){
        if (cmp.getWord(i)){
            cout << "fasle" << endl;

            return false;
        }
    }

    return true;
}

//
//bool LNA::operator>=(const LNA& cmp) const
//{

//}


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
    //if (show){
    //    cout << "is " << *this << endl;
    //    cout << "<= " << cmp << endl << endl;
    //}

    int width = 2 * sizeof(T);
    for (; i != (size_t)-1; i--){
        //if (show){
        //    cout << "mNum " << setfill('0') << setw(width) << std::hex << getWord(i) << endl;
        //    cout << "cmp  " << setfill('0') << setw(width) << std::hex << cmp[i] << endl;
        //}
        if (getWord(i) > cmp[i])
            return false; // strickly greater than
        if (getWord(i) < cmp[i])
            return true; //  strickly less than
    }
    return true; // equal
}

bool LNA::operator<(const LNA& cmp) const
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


void LNA::operator+=(const LNA& op)
{
#ifdef ST   
    string s(":operator+=");
    stackTrace st(s);
#endif
    add(*this, op);
}


void LNA::operator-=(const LNA& op)
{
#ifdef ST    
    string s("operator-=");
    stackTrace st(s);
#endif
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


//LNA& LNA::operator=(const LNA& rhs)
//{
//
//    if (mWordCount < rhs.mWordCount)
//    {
//        delete mNum;
//        mNum = new T[rhs.mWordCount];
//        mWordCount = rhs.mWordCount;
//    }
//    else if (mWordCount > rhs.mWordCount){
//        memset(mNum + rhs.mWordCount, 0, (mWordCount - rhs.mWordCount) * sizeof(T)); // set high words to zero.
//    }
//    memcpy(mNum, rhs.mNum, rhs.mWordCount * sizeof(T));
//    this->retract();
//
//    return *this;
//}


void LNA::randomize(size_t size)
{

    clearResize(size);

    static std::default_random_engine generator;
    static std::uniform_int_distribution<T> distribution(0, (T)-1);
    for (size_t i = 0; i < mWordCount; i++){
        T num = distribution(generator);
        //cout << num << endl;
        getWord(i) = num;
    }

}

std::ostream& operator<< (std::ostream& stream, const  LNA& num)
{

    int width = 2 * sizeof(LNA::T);

    for (size_t i = num.mWordCount - 1; i != (size_t)-1; i--){
        std::cout << std::setfill('0') << std::setw(width) << std::hex << num[i];
    }
    std::cout << "'";

    return stream;
}