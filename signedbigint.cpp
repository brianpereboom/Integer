#include "signedbigint.h"

#include <cstdlib>
#include <cstring>
#include <limits.h>

#define SIGNBIT 128
#define CHAR 256

size_t signedCapacity = 25;

void setSignedBigIntCapacity(size_t cap) {
    signedCapacity = cap;
}

SignedBigInt::SignedBigInt() {
    this->bytes = (unsigned char*)malloc((unsigned char)(signedCapacity));
}

SignedBigInt::SignedBigInt(const SignedBigInt& n) {
    this->bytes = (unsigned char*)malloc(signedCapacity);
    for (unsigned char i = 0; i < signedCapacity; i++){
        this->bytes[i] = n.bytes[i];}
}

SignedBigInt::SignedBigInt(std::string str) {
    *this = toSignedBigInt(str);
}

SignedBigInt::SignedBigInt(char n) {
    this->bytes = (unsigned char*)malloc(signedCapacity);
    bool negative = false;
    if (n & SIGNBIT) {
        negative = true;
        n = ~n + 1;
    }
    this->bytes[0] = n;
    if (negative)
        *this = this->makeNegative();
}

unsigned char* SignedBigInt::operator[] (unsigned char index) {
    try {
        if (index < signedCapacity) {
            return this->bytes + index;
        }
        throw std::runtime_error("Trying to access byte " + std::to_string(index) + " in SignedBigInt of size " + std::to_string(signedCapacity) + ".");
    } catch (const std::exception& e) {
        printf("Index out of range: %s\n", e.what());
    }
    return nullptr;
}

SignedBigInt SignedBigInt::operator++ (int) {
    try {
        if (this->isMax())
            throw std::runtime_error("Reached maximum value.");
        else {
            short carry = 1;
            for (unsigned char i = 0; i < signedCapacity; i++) {
                carry += (short)*(*this)[i];
                *(*this)[i] = (char)carry;
                carry >>= 8;
                if (carry == 0)
                    return *this;
            }
        }
    } catch (const std::exception& e) {
        printf("Increment error: %s\n", e.what());
    }
    return *this;
}

SignedBigInt SignedBigInt::operator-- (int) {
    try {
        if (this->isMin())
            throw std::runtime_error("Reached minimum value.");
        else {
            short carry = 0;
            for (unsigned char i = 0; i < signedCapacity; i++) {
                carry += (short)*(*this)[i] + UCHAR_MAX;
                *(*this)[i] = (char)carry;
                carry >>= 8;
                if (carry == 0)
                    return *this;
            }
        }
    } catch (const std::exception& e) {
        printf("Decrement error: %s\n", e.what());
    }
    return *this;
}

SignedBigInt SignedBigInt::operator+ (char n) {
    SignedBigInt sum = SignedBigInt(*this);
    try {
        short carry = n;
        for (unsigned char i = 0; i < signedCapacity; i++) {
            carry += (short)*(*this)[i];
            *sum[i] = (char)carry;
            carry >>= 8;
            if (carry == 0)
                return sum;
        }
        bool negative[3] = {this->isNegative(), (bool)(n & SIGNBIT), sum.isNegative()};
        if (negative[0] && negative[1] && !negative[2])
            throw std::runtime_error("Reached minimum value.");
        else if (!negative[0] && !negative[1] && negative[2])
            throw std::runtime_error("Reached maximum value.");
    } catch (const std::exception& e) {
        printf("Addition error: %s\n", e.what());
    }
    return sum;
}

SignedBigInt SignedBigInt::operator- (char n) {
    SignedBigInt difference = SignedBigInt(*this);
    try {
        short carry = ~n + 1;
        for (unsigned char i = 0; i < signedCapacity; i++) {
            carry += (short)*(*this)[i];
            *difference[i] = (char)carry;
            carry >>= 8;
            if (carry == 0)
                return difference;
        }
        bool negative[3] = {this->isNegative(), (bool)(n & SIGNBIT), difference.isNegative()};
        if (negative[0] && !negative[1] && !negative[2])
            throw std::runtime_error("Reached minimum value.");
        else if (!negative[0] && negative[1] && negative[2])
            throw std::runtime_error("Reached maximum value.");
    } catch (const std::exception& e) {
        printf("Subtraction error: %s\n", e.what());
    }
    return difference;
}

SignedBigInt SignedBigInt::operator+ (SignedBigInt n) {
    SignedBigInt sum = SignedBigInt();
    try {
        unsigned short carry = 0;
        for (unsigned char i = 0; i < signedCapacity; i++) {
            carry += (unsigned short)*(*this)[i] + (unsigned short)*n[i];
            *sum[i] = (unsigned char)carry;
            carry >>= 8;
        }
        bool negative[3] = {this->isNegative(), n.isNegative(), sum.isNegative()};
        if (negative[0] && negative[1] && !negative[2])
            throw std::runtime_error("Reached minimum value.");
        else if (!negative[0] && !negative[1] && negative[2])
            throw std::runtime_error("Reached maximum value.");
    } catch (const std::exception& e) {
        printf("Addition error: %s\n", e.what());
    }
    return sum;
}

SignedBigInt SignedBigInt::operator- (SignedBigInt n) {
    SignedBigInt difference = SignedBigInt();
    try {
        short carry = 0;
        for (unsigned char i = 0; i < signedCapacity; i++) {
            carry += (short)*(*this)[i] - (short)*n[i];
            *difference[i] = (char)carry;
            carry >>= 8;
        }
        bool negative[3] = {this->isNegative(), n.isNegative(), difference.isNegative()};
        if (negative[0] && !negative[1] && !negative[2])
            throw std::runtime_error("Reached minimum value.");
        else if (!negative[0] && negative[1] && negative[2])
            throw std::runtime_error("Reached maximum value.");
    } catch (const std::exception& e) {
        printf("Subtraction error: %s\n", e.what());
    }
    return difference;
}

SignedBigInt SignedBigInt::operator<< (unsigned char n) {
    SignedBigInt shift = SignedBigInt();
    bool negative = false;
    SignedBigInt num = *this;
    if (this->isNegative()) {
        negative = true;
        num = num.makeNegative();
    }
    unsigned char byteShift = n >> 3;
    unsigned char bitShift = n - (byteShift << 3);
    for (unsigned char i = 0; i < signedCapacity - byteShift; i++)
        *shift[i + byteShift] = (*num[i] << bitShift) | ((i == 0) ? '\0' : (*num[i - 1] >> (8 - bitShift)));
    return negative ? shift.makeNegative() : shift;
}

SignedBigInt SignedBigInt::operator>> (unsigned char n) {
    SignedBigInt shift = SignedBigInt();
    bool negative = false;
    SignedBigInt num = *this;
    if (this->isNegative()) {
        negative = true;
        num = num.makeNegative();
    }
    unsigned char byteShift = n >> 3;
    unsigned char bitShift = n - (byteShift << 3);
    for (unsigned char i = byteShift; i < signedCapacity; i++)
        *shift[i - byteShift] = (*num[i] >> bitShift) | ((i + 1 == signedCapacity) ? '\0' : (*num[i + 1] << (8 - bitShift)));
    return negative ? shift.makeNegative() : shift;
}

SignedBigInt SignedBigInt::operator* (char n) {
    SignedBigInt product = SignedBigInt();
    try {
        bool negative = false;
        SignedBigInt num = *this;
        if (this->isNegative()) {
            negative = true;
            num = num.makeNegative();
        }
        if (n & SIGNBIT) {
            negative = !negative;
            n = ~n + 1;
        }
        unsigned short size = (unsigned short)num.size();
        unsigned int c = n;
        while (c != 0) {
            size++;
            c >>= 1;
        }
        if (size > (signedCapacity << 3) - 1)
            throw std::runtime_error("Product of size " + std::to_string(size) + " exceeds signedCapacity " + std::to_string((signedCapacity << 3) - 1) + ".");
        else {
            unsigned short carry = 0;
            for (unsigned char i = 0; i < signedCapacity; i++) {
                carry += (unsigned short)*num[i] * (unsigned short)n;
                *product[i] = (unsigned char)carry;
                carry >>= 8;
            }
            return negative ? product.makeNegative() : product;
        }
    } catch (const std::exception& e) {
        printf("Multiplication error: %s\n", e.what());
    }
    return product;
}

SignedBigInt SignedBigInt::operator* (SignedBigInt n) {
    SignedBigInt product = SignedBigInt();
    try {
        bool negative = false;
        SignedBigInt num = *this;
        if (this->isNegative()) {
            negative = true;
            num = num.makeNegative();
        }
        if (n.isNegative()) {
            negative = !negative;
            n = n.makeNegative();
        }
        unsigned short size = (unsigned short)num.size() + (unsigned short)n.size();
        if (size > (signedCapacity << 3) - 1)
            throw std::runtime_error("Product of size " + std::to_string(size) + " exceeds signedCapacity " + std::to_string((signedCapacity << 3) - 1) + ".");
        else {
            for (unsigned char i = 0; i < signedCapacity; i++) {
                unsigned char byte = i << 3;
                for (unsigned char j = 0; j < 8; j++)
                    if (*n[i] & (1 << j))
                        product = product + (num << (byte + j));
            }
            return negative ? product.makeNegative() : product;
        }
    } catch (const std::exception& e) {
        printf("Multiplication error: %s\n", e.what());
    }
    return product;
}

SignedBigInt SignedBigInt::operator/ (char n) {
    SignedBigInt quotient = SignedBigInt();
    try {
        if (n == '\0')
            throw std::runtime_error("Division by zero.");
        else {
            bool negative = false;
            SignedBigInt num = *this;
            if (this->isNegative()) {
                negative = true;
                num = num.makeNegative();
            }
            if (n & SIGNBIT) {
                negative = !negative;
                n = ~n + 1;
            }
            unsigned short remainder = 0;
            for (unsigned char i = signedCapacity; i > 0;) {
                i--;
                unsigned short dividend = (unsigned short)*num[i] + remainder;
                *quotient[i] = (unsigned char)(dividend / (unsigned short)n);
                remainder = (dividend - (unsigned short)*quotient[i] * (unsigned short)n) << 8;
            }
            return negative ? quotient.makeNegative() : quotient;
        }
    } catch (const std::exception& e) {
        printf("Division error: %s\n", e.what());
    }
    return quotient;
}

SignedBigInt SignedBigInt::operator/ (SignedBigInt n) {
    SignedBigInt quotient = SignedBigInt();
    try {
        if (n == SignedBigInt())
            throw std::runtime_error("Division by zero.");
        else {
            bool negative = false;
            SignedBigInt num = *this;
            if (this->isNegative()) {
                negative = true;
                num = num.makeNegative();
            }
            if (n.isNegative()) {
                negative = !negative;
                n = n.makeNegative();
            }
            char shift = n.size() - 8;
            if (shift > 0) {
                SignedBigInt p = SignedBigInt(num) >> shift;
                SignedBigInt q = SignedBigInt(n) >> shift;
                quotient = p / *q[0];
                SignedBigInt check = quotient * n;
                if (num > check) {
                    SignedBigInt error = num - check;
                    if (error > n)
                        quotient = quotient + error / n;
                } else if (num < check) {
                    SignedBigInt error = check - num;
                    if (error > n)
                        quotient = quotient - (check - num) / n;
                }
            } else {
                quotient = num / *n[0];
            }
            if (quotient * n > num)
                quotient--;
            return negative ? quotient.makeNegative() : quotient;
        }
    } catch (const std::exception& e) {
        printf("Division error: %s\n", e.what());
    }
    return quotient;
}

SignedBigInt SignedBigInt::operator% (SignedBigInt n) {
    SignedBigInt num = n.isNegative() ? n.makeNegative() : n;
    SignedBigInt modulo = *this - (*this / n) * n;
    return modulo.isNegative() ? modulo + num : modulo;
}

bool SignedBigInt::operator== (SignedBigInt n) {
    for (unsigned char i = signedCapacity; i > 0;) {
        i--;
        if (*(*this)[i] != *n[i])
            return false;
    }
    return true;
}

bool SignedBigInt::operator!= (SignedBigInt n) {
    for (unsigned char i = signedCapacity; i > 0;) {
        i--;
        if (*(*this)[i] != *n[i])
            return true;
    }
    return false;
}

bool SignedBigInt::operator> (SignedBigInt n) {
    bool negative[2] = {this->isNegative(), n.isNegative()};
    if (negative[0] && !negative[1])
        return false;
    if (!negative[0] && negative[1])
        return true;
    if (negative[0] && negative[1]) {
        SignedBigInt left = this->makeNegative();
        SignedBigInt right = n.makeNegative();
        for (unsigned char i = signedCapacity; i > 0;) {
            i--;
            if (*left[i] > *right[i])
                return false;
            if (*left[i] < *right[i])
                return true;
        }
    } else
        for (unsigned char i = signedCapacity; i > 0;) {
            i--;
            if (*(*this)[i] < *n[i])
                return false;
            if (*(*this)[i] > *n[i])
                return true;
        }
    return false;
}

bool SignedBigInt::operator< (SignedBigInt n) {
    bool negative[2] = {this->isNegative(), n.isNegative()};
    if (negative[0] && !negative[1])
        return true;
    if (!negative[0] && negative[1])
        return false;
    if (negative[0] && negative[1]) {
        SignedBigInt left = this->makeNegative();
        SignedBigInt right = n.makeNegative();
        for (unsigned char i = signedCapacity; i > 0;) {
            i--;
            if (*left[i] < *right[i])
                return false;
            if (*left[i] > *right[i])
                return true;
        }
    } else
        for (unsigned char i = signedCapacity; i > 0;) {
            i--;
            if (*(*this)[i] > *n[i])
                return false;
            if (*(*this)[i] < *n[i])
                return true;
        }
    return false;
}

bool SignedBigInt::isMin() {
    for (unsigned char i = 0; i < signedCapacity; i++)
        if (*(*this)[i] != UCHAR_MAX)
            return false;
    return true;
}

bool SignedBigInt::isMax() {
    if (*(*this)[signedCapacity - 1] != UCHAR_MAX ^ SIGNBIT)
        return false;
    for (unsigned char i = 0; i < signedCapacity - 1; i++)
        if (*(*this)[i] != UCHAR_MAX)
            return false;
    return true;
}

bool SignedBigInt::isNegative() {
    return *(*this)[signedCapacity - 1] & SIGNBIT;
}

SignedBigInt SignedBigInt::makeNegative() {
    SignedBigInt negative = SignedBigInt();
    for (unsigned char i = 0; i < signedCapacity; i++)
        *negative[i] = ~*(*this)[i];
    negative++;
    return negative;
}

unsigned char SignedBigInt::size() {
    if (*this == SignedBigInt())
        return 0;
    unsigned char count = signedCapacity - 1;
    unsigned char byte = *(*this)[signedCapacity - 1] & ~SIGNBIT;
    if (byte == '\0') {
        count--;
        for (unsigned char i = signedCapacity - 1; i > 0;) {
            i--;
            byte = *(*this)[i];
            if (byte == '\0')
                count--;
            else
                break;
        }
    }
    count <<= 3;
    while (byte != '\0') {
        count++;
        byte >>= 1;
    }
    return count;
}

SignedBigInt toSignedBigInt(std::string str) {
    SignedBigInt n;
    bool negative = str[0] == '-';
    if (!negative)
        n = SignedBigInt(str[0] - '0');
    for (unsigned char i = 1; i < str.size(); i++) {
        n = n * 10;
        n = n + str[i] - '0';
    }
    return negative ? n.makeNegative() : n;
}

void print(SignedBigInt n) {
    printf("%s\n", to_string(n).c_str());
}

std::string to_string(SignedBigInt n) {
    bool negative = n.isNegative();
    std::string str = "";
    SignedBigInt num = negative ? n.makeNegative() : SignedBigInt(n);
    while (num != SignedBigInt()) {
        SignedBigInt next = num / 10;
        str = std::to_string((unsigned char)*(num - next * 10)[0]) + str;
        num = next;
    }
    if (str == "")
        return "0";
    if (negative)
        return "-" + str;
    return str;
}