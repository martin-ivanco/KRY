#ifndef __RSA__
#define __RSA__

#include <iostream>
#include <fstream>
#include <tuple>
#include <gmpxx.h>

using namespace std;

class RSA {

private:
    static const int MPGI = 50; // maximum prime generation iterations
    static const int MRTI = 32; // Miller-Rabin test iterations

    gmp_randclass rng;

    void seedRNG();
    void generatePrimes(int size);
    bool isPrime(mpz_class n);
    mpz_class gcd(mpz_class n1, mpz_class n2);
    mpz_class inv(mpz_class n1, mpz_class n2);

public:
    mpz_class p, q, n, e, d;

    RSA();
    bool generate(int size);
    mpz_class encrypt(mpz_class message);
    mpz_class decrypt(mpz_class message);

};

#endif // __RSA__