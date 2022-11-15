#ifndef _BIGINT_H_
#define _BIGINT_H_

#include <cstddef>
#include <iostream>

void setBigIntCapacity(size_t capacity);

struct BigInt {
    unsigned char* bytes;

    BigInt();
    BigInt(const BigInt& n);
    BigInt(std::string str);
    BigInt(unsigned char n);
    ~BigInt() {
        free(this->bytes);
    }

    unsigned char* operator[] (unsigned char index);
    BigInt operator++ (int);
    BigInt operator-- (int);
    BigInt operator+ (BigInt n);
    BigInt operator- (BigInt n);
    BigInt operator<< (unsigned char n);
    BigInt operator>> (unsigned char n);
    BigInt operator* (BigInt n);
    BigInt operator/ (BigInt n);
    BigInt operator* (unsigned char n);
    BigInt operator/ (unsigned char n);
    BigInt operator% (BigInt n);

    bool operator== (BigInt n);
    bool operator!= (BigInt n);
    bool operator> (BigInt n);
    bool operator< (BigInt n);

    unsigned char size();
};

BigInt toBigInt(std::string str);

void print(BigInt n);

std::string to_string(BigInt n);

#endif