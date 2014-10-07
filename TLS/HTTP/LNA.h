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

    static void add(LNA<T>& target, LNA<T>& source);

    void resize(uint32_t);

    uint32_t mWorkCount;
    T* mNum;
};


template<class T>
std::ostream& operator << (std::ostream& stream, const  LNA<T>& num);