#include <iostream>
#include <tuple>
#include <gmpxx.h>

#include "argparse.hpp"
#include "rsa.hpp"

using namespace std;

int main(int argc, char **argv) {
    // Parse arguments
    ArgParse arg(argc, argv);
    if (! arg.parse()) 
        return 1;

    // Create RSA
    RSA rsa = RSA();

    if (arg.task == ArgParse::GENERATE) {
        if (! rsa.generate(arg.size))
            return 2;
        cout << "0x" + rsa.p.get_str(16) + " 0x" + rsa.q.get_str(16) + " 0x" + rsa.n.get_str(16)
                + " 0x" + rsa.e.get_str(16) + " 0x" + rsa.d.get_str(16) << endl;
    }

    if (arg.task == ArgParse::ENCRYPT) {
        rsa.e = arg.exponent;
        rsa.n = arg.modulo;
        mpz_class result = rsa.encrypt(arg.message);
        cout << "0x" + result.get_str(16) << endl;
    }

    if (arg.task == ArgParse::DECRYPT) {
        rsa.d = arg.exponent;
        rsa.n = arg.modulo;
        mpz_class result = rsa.decrypt(arg.message);
        cout << "0x" + result.get_str(16) << endl;
    }

    if (arg.task == ArgParse::BREAK) {
        cerr << "Sorry, breaking cyphers is not yet implemented." << endl;
        return 3;
    }

    return 0;
}
