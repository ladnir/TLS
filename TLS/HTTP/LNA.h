#pragma once
#include <stdint.h>
#include <vector>

#include <cassert>
#include <string.h>
#include <ostream>
#include <iomanip>
#include <random>
#include <iostream>


class LNA
{
public:
    static bool show;
    typedef uint64_t T;
    enum WordOrder {LSWF, MSWF};

    LNA(const T);

    //template<class type>
    LNA(WordOrder, const std::vector<uint8_t>&);

    //template<class type>
    //LNA(WordOrder, 
    //    typename std::vector<type>::const_iterator&,
    //    typename std::vector<type>::const_iterator&);

    LNA(WordOrder,
        std::vector<uint8_t>::const_iterator&,
        std::vector<uint8_t>::const_iterator&);

	LNA(const LNA&);
	LNA();
    ~LNA();

    //template<class type>
    void vectorInit(WordOrder, 
         std::vector<uint8_t>::const_iterator&,
         std::vector<uint8_t>::const_iterator&);

    static void add(LNA& target,const LNA& source);
	static void subtract(LNA& target, const LNA& source);

	static void multiply(const LNA& multiplicand, 
						 const LNA& multiplier, 
							   LNA& product);

    static void exponentiate(      LNA& base,
                             const LNA& exponent);

    static void modExponentiate(const LNA& base,
                                const LNA& exponent,
                                const LNA& modulus,
                                      LNA& remainder);

    static void secureModExponentiate(const LNA& base,
                                      const LNA& exponent,
                                      const LNA& modulus,
                                            LNA& remainder);

    static void mod(      LNA& dividendRemainder,
                    const LNA& modulus);

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
    size_t byteLength() const;

    T& getWord(const size_t idx)const;

    void unload(WordOrder wo, size_t bytes, std::vector<uint8_t>& destination);

	T&      operator[]  (const size_t&)const;
    //bool    operator>=  (const LNA&)const;
    bool    operator<=  (const LNA&)const;
    bool    operator<  (const LNA&)const;
    bool    operator==  (const LNA&)const;
    void    operator=   (const LNA&);
    void    operator-=  (const LNA&);
    void    operator+=  (const LNA&);
    void    operator*=  (const LNA&);
    void    operator<<= (const size_t& bits);
    void    operator>>= (const size_t& bits);
    LNA&    operator++();
    void    operator%=(const LNA&);

    void randomize(size_t size);

    size_t mWordCount;
	size_t mWordSize;

    T* mNum;


    static const LNA one;
};


std::ostream& operator<< (std::ostream& stream, const  LNA& num);

//template<class R>
//std::vector<R> operator<<(std::vector<R>& stream, const LNA& num);

