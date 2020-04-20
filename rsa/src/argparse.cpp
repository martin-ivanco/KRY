#include "argparse.hpp"

using namespace std;

const string ArgParse::HELP_MESSAGE = 
    "usage: ./run.sh run [-h] [-g <size>] [-e <public exp> <modulo> <message>]\n"
    "                    [-d <private exp> <modulo> <cypher>]\n"
    "                    [-b <public exp> <modulo> <cypher>]\n"
    "\n"
    "\e[1mRSA Utility\e[0m\n"
    "\n"
    "This application implements RSA key generation (-g), message encrypting (-e)\n"
    "and decrypting (-d), and cypher breaking (-b) exploiting weak public modulo.\n"
    "\n"
    "optional arguments:\n"
    "  -h, --help       show this help message and exit\n"
    "  -g, --generate   generate key with given size of public modulo in num of bits\n"
    "  -e, --encrypt    encrypt given message using public exponent and modulo\n"
    "  -d, --decrypt    decrypt given cypher using private exponent and modulo\n"
    "  -b, --break      break given cypher using public exponent and modulo";

ArgParse::ArgParse(int argc, char **argv) {
    // Move arguments to a vector
    for (int i = 1; i < argc; i++)
        this->args.push_back(argv[i]);
}

bool ArgParse::parse() {
    // Loop through all arguments
    for (int i = 0; i < this->args.size(); i++) {
        // Print help
        if ((this->args[i] == string("-h")) || (this->args[i] == string("--help"))) {
            cout << ArgParse::HELP_MESSAGE << endl;
            return false;
        }

        // Generate key
        if ((this->args[i] == string("-g")) || (this->args[i] == string("--generate"))) {
            if (! this->checkValues(i, 1))
                return false;
            this->task = ArgParse::GENERATE;
            i += 1;
            this->size = stoi(this->args[i]);
            continue;
        }

        // Encrypt message
        if ((this->args[i] == string("-e")) || (this->args[i] == string("--encrypt"))) {
            if (! this->checkValues(i, 3))
                return false;
            this->task = ArgParse::ENCRYPT;
            i += 1;
            this->exponent = mpz_class(this->args[i]);
            i += 1;
            this->modulo = mpz_class(this->args[i]);
            i += 1;
            this->message = mpz_class(this->args[i]);
            continue;
        }

        // Decrypt cypher
        if ((this->args[i] == string("-d")) || (this->args[i] == string("--decrypt"))) {
            if (! this->checkValues(i, 3))
                return false;
            this->task = ArgParse::DECRYPT;
            i += 1;
            this->exponent = mpz_class(this->args[i]);
            i += 1;
            this->modulo = mpz_class(this->args[i]);
            i += 1;
            this->message = mpz_class(this->args[i]);
            continue;
        }

        // Break cypher
        if ((this->args[i] == string("-b")) || (this->args[i] == string("--break"))) {
            if (! this->checkValues(i, 3))
                return false;
            this->task = ArgParse::BREAK;
            i += 1;
            this->exponent = mpz_class(this->args[i]);
            i += 1;
            this->modulo = mpz_class(this->args[i]);
            i += 1;
            this->message = mpz_class(this->args[i]);
            continue;
        }

        // Invalid argument
        cerr << string("Invalid argument ") + this->args[i] + string(".") << endl;
        return false;
    }

    // Check if task was chosen
    if (this->task == ArgParse::UNASSIGNED) {
        cerr << "No task was chosen." << endl;
        return false;
    }

    return true;
}

bool ArgParse::checkValues(int idx, int count) {
    if (this->args.size() < idx + count + 1) {
        cerr << string("Argument ") + this->args[idx] + string(" needs ") + to_string(count)
                + string(" values.") << endl;
        return false;
    }

    return true;
}
