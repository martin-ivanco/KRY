#ifndef __ARGPARSE__
#define __ARGPARSE__

#include <iostream>
#include <vector>
#include <gmpxx.h>

using namespace std;

class ArgParse {

private:
    static const string HELP_MESSAGE;

    vector<string> args;

    bool checkValues(int idx, int count);

public:
    static const int UNASSIGNED = -1;

    // Tasks
    static const int GENERATE = 0;
    static const int ENCRYPT = 1;
    static const int DECRYPT = 2;
    static const int BREAK = 3;

    int task = UNASSIGNED;
    int size;
    mpz_class exponent;
    mpz_class modulo;
    mpz_class message;

    ArgParse(int argc, char **argv);
    bool parse();

};

#endif // __ARGPARSE__