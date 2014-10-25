#pragma once
#include <stdint.h>
#include <vector>

#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>
#include <random>
#include "stackTrace.h"
#include <iostream>


class LNA
{
public:
    static bool show;
    typedef uint64_t T;


    LNA(const std::vector<T>&);
	LNA(const LNA&);
	LNA();
    ~LNA();

    static void add(LNA& target,const LNA& source);
	static void subtract(LNA& target, const LNA& source);

	static void multiply(const LNA& multiplicand, 
						 const LNA& multiplier, 
							   LNA& product);

    static void division(const LNA& dividend,
						 const LNA& divisor,
							   LNA& quotient,
							   LNA& remainder);

	void expand(size_t minSize);
	void retract();
    void clearResize(size_t size);
    void clear();
    bool isZero() const;
    void copy(const LNA&);
    T& getWord(const size_t idx)const;

	T&      operator[]  (const size_t&)const;
    //bool    operator>=  (const LNA&)const;
    bool    operator<=  (const LNA&)const;
    bool    operator<  (const LNA&)const;
    bool    operator==  (const LNA&)const;
    //LNA& operator=   (const LNA&);
    void    operator-=  (const LNA&);
    void    operator+=  (const LNA&);
    void    operator*=  (const LNA&);
    void    operator<<= (const size_t& bits);
    void    operator>>= (const size_t& bits);

    void randomize(size_t size);

    size_t mWordCount;
	size_t mWordSize;

    T* mNum;
};


std::ostream& operator<< (std::ostream& stream, const  LNA& num);

