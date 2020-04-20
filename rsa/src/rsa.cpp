#include "rsa.hpp"

RSA::RSA() : rng(gmp_randinit_mt) {
    this->seedRNG();
}

void RSA::generate(int size) {
    // TODO check size
    
    // Find a suitable pair of random primes (p, q) to make modulo n
    this->n = 0;
    while (mpz_sizeinbase(n.get_mpz_t(), 2) != size) {
        this->generatePrimes(size);
        this->n = this->p * this->q;
    }

    // Compute phi(n)
    mpz_class phi = (this->p - 1) * (this->q - 1);

    // Find 1 < e < phi(n), such that gcd(e, phi(n)) = 1
    this->e = phi;
    while(this->gcd(this->e, phi) != 1) {
        this->e = this->rng.get_z_range(phi - 2) + 2;
    }
    
    // Find d = inv(e, phi(n))
    this->d = this->inv(this->e, phi);
}

mpz_class RSA::encrypt(mpz_class message) {
    mpz_class result;
    mpz_powm(result.get_mpz_t(), message.get_mpz_t(), this->e.get_mpz_t(), this->n.get_mpz_t());
    return result;
}

mpz_class RSA::decrypt(mpz_class message) {
    mpz_class result;
    mpz_powm(result.get_mpz_t(), message.get_mpz_t(), this->d.get_mpz_t(), this->n.get_mpz_t());
    return result;
}

void RSA::seedRNG() {
    ifstream urandom("/dev/urandom", ios::in|ios::binary);
    if (urandom) {
        unsigned long seed;
        urandom.read(static_cast<char *>(static_cast<void *>(&seed)), sizeof(seed));
        if (urandom) {
            this->rng.seed(seed);
        }
        else {
            cerr << "ERROR: Could not read from /dev/urandom. Keys will be insecure.";
            this->rng.seed(0);
        }
    }
    else {
        cerr << "ERROR: Could not open /dev/urandom. Keys will be insecure.";
        this->rng.seed(0);
    }
}

void RSA::generatePrimes(int size) {
    // Generate two random numbers
    this->p = this->rng.get_z_bits(size / 2 + size % 2);
    this->q = this->rng.get_z_bits(size / 2 + size % 2);

    // Make them odd
    mpz_setbit(this->p.get_mpz_t(), 0);
    mpz_setbit(this->q.get_mpz_t(), 0);
    
    // Make sure they are different
    while (this->p == this->q) {
        this->q = this->rng.get_z_bits(size / 2 + size % 2);
        mpz_setbit(this->q.get_mpz_t(), 0);
    }

    // Increment them by 2 until they become prime
    while (! this->isPrime(this->p)) {
        this->p += 2;
    }

    // Also make sure they are still different
    while ((! this->isPrime(this->q)) || (this->p == this->q)) {
        this->q += 2;
    }
}

bool RSA::isPrime(mpz_class n) {
    // Count d and r
    mpz_class d = n - 1;
    mpz_class r = 0;
    while ((d % 2) < 1) {
        d /= 2;
        r += 1;
    }

    // Main loop of Miller-Rabin test
    mpz_class a, x, t;
    mpz_class s = 2;
    for (int i = 0; i < RSA::MRTI; i++) {
        a = this->rng.get_z_range(n - 3) + 2;
        mpz_powm(x.get_mpz_t(), a.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
        if ((x == 1) or (x == n - 1))
            continue;

        for (int j = 0; j < r - 1; j++) {
            mpz_powm(t.get_mpz_t(), x.get_mpz_t(), s.get_mpz_t(), n.get_mpz_t());
            x = t;
            if (x == n - 1)
                continue;
        }

        return false;
    }

    // The number is most probably prime
    return true;
}

mpz_class RSA::gcd(mpz_class n1, mpz_class n2) {
    mpz_class a = n1;
    mpz_class b = n2;
    mpz_class t;

    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    }

    return a;
}

mpz_class RSA::inv(mpz_class n1, mpz_class n2) {
    mpz_class t = 0;
    mpz_class nt = 1;
    mpz_class r = n2;
    mpz_class nr = n1;
    mpz_class q, s;
    
    while(nr != 0) {
        q = r / nr;
        s = nt;
        nt = t - (q * nt);
        t = s;
        s = nr;
        nr = r - (q * nr);
        r = s;
    }

    if (r > 1)
        throw "n1 is not invertible";
    if (t < 0)
        t += n2;

    return t;
}
