#pragma once

#include "LNA.h"
#include <vector>
#include <stdint.h>

class RSA
{
   


public:
    typedef struct {
        LNA modulus;
        LNA publicKey;
        LNA privateKey;
    } Key;

    RSA(const Key&);
    RSA(const LNA& modulus, const LNA& publicKey, const LNA& privateKey);
    ~RSA();

    void encrypt(const std::vector<uint8_t>& input,
                       std::vector<uint8_t>& output);

    void decrypt(const std::vector<uint8_t>& input,
                       std::vector<uint8_t>& output);

    static void compute(const LNA& m,
                        const LNA& e,
                        const LNA& n, 
                              LNA& c);

private:
    Key mKey;


    static const size_t paddingLen;
};

