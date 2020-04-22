#ifndef __RSA__
#define __RSA__

#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>
#include <gmpxx.h>

using namespace std;

class RSA {

typedef tuple<mpz_class, mpz_class, mpz_class> mpz_tuple;

private:
    static const int MPGI = 50; // maximum prime generation iterations
    static const int MRTI = 32; // Miller-Rabin test iterations
    static const int PL = 100000; // Primes limit

    gmp_randclass rng;

    void seedRNG();
    void generatePrimes(int size);
    bool isPrime(mpz_class n);
    mpz_class gcd(mpz_class n1, mpz_class n2);
    mpz_class inv(mpz_class n1, mpz_class n2, bool pos = true);
    mpz_class rho(mpz_class n);
    mpz_class ecm(mpz_class n);
    void soe(vector<mpz_class> &primes);
    mpz_tuple emul(mpz_class k, mpz_tuple p, mpz_class a, mpz_class b, mpz_class m);
    mpz_tuple eadd(mpz_tuple p, mpz_tuple q, mpz_class a, mpz_class b, mpz_class m);

public:
    mpz_class p, q, n, e, d;

    RSA();
    bool generate(int size);
    mpz_class encrypt(mpz_class message);
    mpz_class decrypt(mpz_class message);
    mpz_class breakWeakN(mpz_class message);

};

#endif // __RSA__