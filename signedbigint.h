#ifndef _SIGNEDBIGINT_H_
#define _SIGNEDBIGINT_H_

#include <cstddef>
#include <iostream>

void setSignedBigIntCapacity(size_t cap);

struct SignedBigInt {
    unsigned char* bytes;

    SignedBigInt();
    SignedBigInt(const SignedBigInt& n);
    SignedBigInt(std::string str);
    SignedBigInt(char n);
    ~SignedBigInt() {
        free(this->bytes);
    }

    unsigned char* operator[] (unsigned char index);
    SignedBigInt operator++ (int);
    SignedBigInt operator-- (int);
    SignedBigInt operator+ (char n);
    SignedBigInt operator- (char n);
    SignedBigInt operator+ (SignedBigInt n);
    SignedBigInt operator- (SignedBigInt n);
    SignedBigInt operator<< (unsigned char n);
    SignedBigInt operator>> (unsigned char n);
    SignedBigInt operator* (SignedBigInt n);
    SignedBigInt operator/ (SignedBigInt n);
    SignedBigInt operator* (char n);
    SignedBigInt operator/ (char n);
    SignedBigInt operator% (SignedBigInt n);

    bool operator== (SignedBigInt n);
    bool operator!= (SignedBigInt n);
    bool operator> (SignedBigInt n);
    bool operator< (SignedBigInt n);

    bool isMin();
    bool isMax();
    bool isNegative();
    SignedBigInt makeNegative();
    unsigned char size();
};

SignedBigInt toSignedBigInt(std::string str);

void print(SignedBigInt n);

std::string to_string(SignedBigInt n);

#endif