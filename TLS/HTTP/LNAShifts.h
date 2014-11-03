#pragma once
#include "LNA.h"


class LNAShifts :
    public LNA
{
public:
    size_t mRealWordCount;
    size_t mWordsAdded;

    T* mShifts[8];
    
    LNAShifts(const LNA&, size_t, std::string);
    ~LNAShifts();

    void goToShift(size_t shifts);
};

