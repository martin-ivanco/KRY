#include "rsa.hpp"

using namespace std;

RSA::RSA() : rng(gmp_randinit_mt) {
    this->seedRNG();
}

bool RSA::generate(int size) {
    // Check if size is valid
    if (size < 3) {
        cerr << "ERROR: Invalid size. Choose a size of at least 3." << endl;
        return false;
    }

    // Find a suitable pair of random primes (p, q) to make modulo n
    this->n = 0;
    for (int i = 0; i < RSA::MPGI; i++) {
        this->generatePrimes(size);
        this->n = this->p * this->q;
        if (mpz_sizeinbase(n.get_mpz_t(), 2) == size)
            break;
    }
    if (mpz_sizeinbase(n.get_mpz_t(), 2) != size) {
        cerr << "ERROR: Couldn't find suitable pair of primes. Try setting a larger size." << endl;
        return false;
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

    return true;
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

mpz_class RSA::breakWeakN(mpz_class message) {
    // Factorize n to find p and q
    this->p = this->ecm(this->n);
    this->q = this->n / this->p;

    // Find d = inv(e, phi(n))
    this->d = this->inv(this->e, (this->p - 1) * (this->q - 1));

    // Decrypt message
    return this->decrypt(message);
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
    // Check special cases
    if (n < 2)
        return false;
    if (n < 4)
        return true;

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

mpz_class RSA::inv(mpz_class n1, mpz_class n2, bool pos) {
    // Find modular inverse
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
    if (pos && (t < 0))
        t += n2;

    return t;
}

mpz_class RSA::rho(mpz_class n) {
    // Pollard's rho factorization algorithm
    mpz_class x, y, g;

    while (true) {
        x = this->rng.get_z_range(n - 2) + 2;
        y = x;
        g = 1;

        while (g == 1) {
            // g(x) = (x * x - 1) % n
            x *= x; x += 1; x %= n;
            y *= y; y += 1; y %= n;
            y *= y; y += 1; y %= n;
            g = gcd(abs(x - y), n);
        }

        if (g != n)
            return g;
    }
}

mpz_class RSA::ecm(mpz_class n) {
    // Lenstra elliptic curve factorization
    vector<mpz_class> primes;
    this->soe(primes);
    mpz_class a, b, g, pp;
    mpz_tuple q;

    while (true) {
        g = n;
        while (g == n) {
            get<0>(q) = this->rng.get_z_range(n);
            get<1>(q) = this->rng.get_z_range(n);
            get<2>(q) = 1;
            a = this->rng.get_z_range(n);
            b = ((get<1>(q) * get<1>(q)) - (get<0>(q) * get<0>(q) * get<0>(q)) - (a * get<0>(q)));
            b %= n; if (b < 0) b += n;
            g = gcd((4 * a * a * a) + (27 * b * b), n);
        }

        if (g > 1)
            return g;

        for (mpz_class p : primes) {
            pp = p;
            while (pp < RSA::PL) {
                q = this->emul(p, q, a, b, n);
                if (get<2>(q) > 1)
                    return gcd(get<2>(q), n);
                pp = p * pp;
            }
        }
    }
}

void RSA::soe(vector<mpz_class> &primes) {
    // Sieve of Eratosthenes
    vector<bool> bs(RSA::PL, true);
    for (int p = 2; p <= RSA::PL; p++) {
        if (bs[p]) {
            primes.push_back(p);
            for (int i = p; i <= RSA::PL; i += p)
                bs[i] = false;
        }
    }
}

RSA::mpz_tuple RSA::emul(mpz_class k, mpz_tuple p, mpz_class a, mpz_class b, mpz_class m) {
    // Elliptic multiplication
    mpz_tuple r(0, 1, 0);
    while (k > 0) {
        if (get<2>(p) > 1)
            return p;
        if (k % 2 == 1)
            r = this->eadd(p, r, a, b, m);
            
        k /= 2;
        p = this->eadd(p, p, a, b, m);
    }
    return r;
}

RSA::mpz_tuple RSA::eadd(mpz_tuple p, mpz_tuple q, mpz_class a, mpz_class b, mpz_class m) {
    // Elliptic addition
    mpz_class n, d, i, z;
    if (get<2>(p) == 0) return q;
    if (get<2>(q) == 0) return p;

    if (get<0>(p) == get<0>(q)) {
        if ((get<1>(p) + get<1>(q)) % m == 0)
            return mpz_tuple(0, 1, 0);
        n = (3 * get<0>(p) * get<0>(p) + a) % m; if (n < 0) n += m;
        d = (2 * get<1>(p)) % m; if (d < 0) d += m;
    }
    else {
        n = (get<1>(q) - get<1>(p)) % m; if (n < 0) n += m;
        d = (get<0>(q) - get<0>(p)) % m; if (d < 0) d += m;
    }

    try {
        i = this->inv(d, m, false);
    }
    catch (...) {
        return mpz_tuple(0, 0, d);
    }

    z = (n * i * n * i - get<0>(p) - get<0>(q)) % m; if (z < 0) z += m;
    d = (n * i * (get<0>(p) - z) - get<1>(p)) % m; if (d < 0) d += m;
    return mpz_tuple(z, d, 1);
}
