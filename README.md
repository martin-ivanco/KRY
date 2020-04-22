# KRY
Projects for Cryptography - One-Time Pad and RSA
# One-Time Pad and RSA
Two projects made as a part of the course Cryptography at Brno University of Technology. The task of the first project was to crack the One-Time Pad algorithm weakened by reusing the same key multiple times. The task of the second project was to create an RSA Utility capable of generating keys for RSA as well as encrypt and decrypt messages encoded to hexadecimal numbers. It is also able to crack keys with short length (under 128 bits in resonable amount of time ~ under a minute) using Lenstra's Elliptic Curve Factorization algorithm. It also implements Pollard's Rho Factorization algorithm, which can be found in the `src/rsa.cpp` file, but by default, the implementation uses elliptic curves as they are much faster for longer (over 64 bits) keys.

## How to use
Clone this repo using `git clone https://github.com/photohunter9/KRY.git`
* Weakened One-Time Pad:
  1. Run the app using `python otp.py -h` to get info on how to crack key
  2. If you're missing some dependencies, get them using `pip`
  3. ???
  4. Profit!
* RSA Utily:
  1. Build the app using `./run.sh build`
  2. Run the app using `./run.sh run -h` to get info how to generate keys, encrypt or decrypt messages as well as crack private key
  3. ???
  4. Profit!

## Dependencies
* Python 3
* C++17
* CMake
