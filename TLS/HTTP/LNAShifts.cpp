#include "LNAShifts.h"


LNAShifts::LNAShifts(const LNA& src, size_t shifts, std::string shiftDir)
{
    delete mNum;

    if (shiftDir == "<<"){
        mWordsAdded = (shifts + sizeof(T)* 8 - 1) / (sizeof(T)* 8);
        mRealWordCount = src.mWordCount + mWordsAdded;

        size_t pow2 = 1;
        while (pow2 < mRealWordCount){
            pow2 <<= 1;
        }
        mRealWordCount = pow2;

        mWordCount = mRealWordCount;

        for (int i = 0 ; i < 8; i++){

            mShifts[i] = new T[mRealWordCount + 1]();
            memcpy(mShifts[i] + mWordsAdded, src.mNum, src.mWordCount * sizeof(T));
            
            mNum = mShifts[i];

            LNA::operator>>=(i);
        }

        goToShift(shifts);

    }
    else{
        assert(0);
    }

}


LNAShifts::~LNAShifts()
{
    for (int i = 0; i < 8; i++){
        delete mShifts[i];
    }
    mNum = nullptr;
}


void LNAShifts::goToShift(size_t shifts)
{
    size_t bitShifts = shifts & 7; // shifts % 8
    size_t idx = (8 - bitShifts) & 7;

    size_t byteShifts = shifts % (sizeof(T) * 8); 
    size_t byteIdx = (byteShifts + 7) / 8;

    size_t wordOffset = (shifts % mWordSize) ? 1 : 0;
    size_t aw = (shifts + (sizeof(T)* 8) - 1) / (sizeof(T)* 8);

    assert(aw <= mWordsAdded);


    mNum = (T*)(((uint8_t*)(mShifts[idx])) - byteIdx) + wordOffset - aw + mWordsAdded;

    mWordCount = mRealWordCount + (aw - mWordsAdded);

}
