#include "bigint.h"

#include <cstdlib>
#include <cstring>
#include <limits.h>

#define CHAR 256

size_t capacity = 25;

void setBigIntCapacity(size_t cap) {
    capacity = cap;
}

BigInt::BigInt() {
    this->bytes = (unsigned char*)malloc(capacity);
}

BigInt::BigInt(const BigInt& n) {
    this->bytes = (unsigned char*)malloc(capacity);
    for (unsigned char i = 0; i < capacity; i++)
        this->bytes[i] = n.bytes[i];
}

BigInt::BigInt(std::string str) {
    *this = toBigInt(str);
}

BigInt::BigInt(unsigned char n) {
    this->bytes = (unsigned char*)malloc(capacity);
    this->bytes[0] = n;
}

unsigned char* BigInt::operator[] (unsigned char index) {
    try {
        if (index < capacity) {
            return this->bytes + index;
        }
        throw std::runtime_error("Trying to access byte " + std::to_string(index) + " in BigInt of size " + std::to_string(capacity) + ".");
    } catch (const std::exception& e) {
        printf("Index out of range: %s\n", e.what());
    }
    return nullptr;
}

BigInt BigInt::operator++ (int) {
    try {
        unsigned short carry = 1;
        for (unsigned char i = 0; i < capacity; i++) {
            carry += (unsigned short)*(*this)[i];
            *(*this)[i] = (unsigned char)carry;
            carry >>= 8;
            if (carry == 0)
                return *this;
        }
        throw std::runtime_error("Cannot increment past capacity.");
    } catch (const std::exception& e) {
        printf("Addition error: %s\n", e.what());
    }
    *this = BigInt();
    return *this;
}

BigInt BigInt::operator-- (int) {
    try {
        if (*this == BigInt())
            throw std::runtime_error("Cannot decrement 0.");
        else {
            short carry = -1;
            for (unsigned char i = 0; i < capacity; i++) {
                carry += (short)*(*this)[i];
                if (carry < 0) {
                    carry += CHAR;
                    *(*this)[i] = (unsigned char)carry + CHAR;
                    carry = -1;
                } else {
                    *(*this)[i] = (unsigned char)carry;
                    carry = 0;
                }
                if (carry == 0)
                    return *this;
            }
            return *this;
        }
    } catch (const std::exception& e) {
        printf("Subtraction error: %s\n", e.what());
    }
    return *this;
}

BigInt BigInt::operator+ (BigInt n) {
    BigInt sum = BigInt();
    try {
        unsigned short carry = 0;
        for (unsigned char i = 0; i < capacity; i++) {
            carry += (unsigned short)*(*this)[i] + (unsigned short)*n[i];
            *sum[i] = (unsigned char)carry;
            carry >>= 8;
        }
        return sum;
        if (carry == 0)
            return sum;
        throw std::runtime_error("Sum exceeds capacity of " + std::to_string(capacity) + " bytes.");
    } catch (const std::exception& e) {
        printf("Addition error: %s\n", e.what());
    }
    sum = BigInt();
    return sum;
}

BigInt BigInt::operator- (BigInt n) {
    BigInt difference = BigInt();
    try {
        short carry = 0;
        for (unsigned char i = 0; i < capacity; i++) {
            carry += (short)*(*this)[i] - (short)*n[i];
            if (carry < 0) {
                carry += CHAR;
                *difference[i] = (unsigned char)carry + CHAR;
                carry = -1;
            } else {
                *difference[i] = (unsigned char)carry;
                carry = 0;
            }
        }
        if (carry == 0)
            return difference;
        throw std::runtime_error("Difference is negative.");
    } catch (const std::exception& e) {
        printf("Subtraction error: %s\n", e.what());
    }
    difference = BigInt();
    return difference;
}

BigInt BigInt::operator<< (unsigned char n) {
    BigInt shift = BigInt();
    unsigned char byteShift = n >> 3;
    unsigned char bitShift = n - (byteShift << 3);
    for (unsigned char i = 0; i < capacity - byteShift; i++)
        *shift[i + byteShift] = (*(*this)[i] << bitShift) | ((i == 0) ? '\0' : (*(*this)[i - 1] >> (8 - bitShift)));
    return shift;
}

BigInt BigInt::operator>> (unsigned char n) {
    BigInt shift = BigInt();
    unsigned char byteShift = n >> 3;
    unsigned char bitShift = n - (byteShift << 3);
    for (unsigned char i = byteShift; i < capacity; i++)
        *shift[i - byteShift] = (*(*this)[i] >> bitShift) | ((i + 1 == capacity) ? '\0' : (*(*this)[i + 1] << (8 - bitShift)));
    return shift;
}

BigInt BigInt::operator* (unsigned char n) {
    BigInt product = BigInt();
    try {
        unsigned short size = (unsigned short)this->size() + 8;
        if (size > capacity << 3)
            throw std::runtime_error("Product of size " + std::to_string(this->size() + 8) + " exceeds capacity " + std::to_string(capacity << 3) + ".");
        else {
            unsigned short carry = 0;
            for (unsigned char i = 0; i < capacity; i++) {
                carry += (unsigned short)*(*this)[i] * (unsigned short)n;
                *product[i] = (unsigned char)carry;
                carry >>= 8;
            }
            return product;
        }
    } catch (const std::exception& e) {
        printf("Multiplication error: %s\n", e.what());
    }
    return product;
}

BigInt BigInt::operator* (BigInt n) {
    BigInt product = BigInt();
    try {
        unsigned short size = (unsigned short)this->size() + (unsigned short)n.size();
        if (size > capacity << 3)
            throw std::runtime_error("Product of size " + std::to_string(this->size() + n.size()) + " exceeds capacity " + std::to_string(capacity << 3) + ".");
        else {
            for (unsigned char i = 0; i < capacity; i++) {
                unsigned char byte = i << 3;
                for (unsigned char j = 0; j < 8; j++)
                    if (*n[i] & (1 << j))
                        product = product + (*this << (byte + j));
            }
            return product;
        }
    } catch (const std::exception& e) {
        printf("Multiplication error: %s\n", e.what());
    }
    return product;
}

BigInt BigInt::operator/ (unsigned char n) {
    BigInt quotient = BigInt();
    try {
        if (n == '\0')
            throw std::runtime_error("Division by zero.");
        else {
            unsigned short remainder = 0;
            for (unsigned char i = capacity; i > 0;) {
                i--;
                unsigned short dividend = (unsigned short)*(*this)[i] + remainder;
                *quotient[i] = (unsigned char)(dividend / (unsigned short)n);
                remainder = (dividend - (unsigned short)*quotient[i] * (unsigned short)n) << 8;
            }
            return quotient;
        }
    } catch (const std::exception& e) {
        printf("Division error: %s\n", e.what());
    }
    return quotient;
}

BigInt BigInt::operator/ (BigInt n) {
    BigInt quotient = BigInt();
    try {
        if (n == BigInt())
            throw std::runtime_error("Division by zero.");
        else {
            char shift = n.size() - 8;
            if (shift > 0) {
                BigInt p = BigInt(*this) >> shift;
                BigInt q = BigInt(n) >> shift;
                quotient = p / *q[0];
                BigInt check = quotient * n;
                if (*this > check) {
                    BigInt error = *this - check;
                    if (error > n)
                        quotient = quotient + error / n;
                } else if (*this < check) {
                    BigInt error = check - *this;
                    if (error > n)
                        quotient = quotient - (check - *this) / n;
                }
            } else {
                quotient = *this / *n[0];
            }
            if (quotient * n > *this)
                quotient--;
            return quotient;
        }
    } catch (const std::exception& e) {
        printf("Division error: %s\n", e.what());
    }
    return quotient;
}

BigInt BigInt::operator% (BigInt n) {
    return *this - (*this / n) * n;
}

bool BigInt::operator== (BigInt n) {
    for (unsigned char i = capacity; i > 0;) {
        i--;
        if (*(*this)[i] != *n[i])
            return false;
    }
    return true;
}

bool BigInt::operator!= (BigInt n) {
    for (unsigned char i = capacity; i > 0;) {
        i--;
        if (*(*this)[i] != *n[i])
            return true;
    }
    return false;
}

bool BigInt::operator> (BigInt n) {
    for (unsigned char i = capacity; i > 0;) {
        i--;
        if (*(*this)[i] < *n[i])
            return false;
        if (*(*this)[i] > *n[i])
            return true;
    }
    return false;
}

bool BigInt::operator< (BigInt n) {
    for (unsigned char i = capacity; i > 0;) {
        i--;
        if (*(*this)[i] > *n[i])
            return false;
        if (*(*this)[i] < *n[i])
            return true;
    }
    return false;
}

unsigned char BigInt::size() {
    if (*this == BigInt())
        return 0;
    unsigned char count = capacity - 1;
    unsigned char byte = '\0';
    for (unsigned char i = capacity; i > 0;) {
        i--;
        byte = *(*this)[i];
        if (byte == '\0')
            count--;
        else
            break;
    }
    count <<= 3;
    while (byte != '\0') {
        count++;
        byte >>= 1;
    }
    return count;
}

BigInt toBigInt(std::string str) {
    BigInt n = BigInt((unsigned char)str[0] - '0');
    for (unsigned char i = 1; i < str.size(); i++) {
        n = n * 10;
        n = n + (str[i] - '0');
    }
    return n;
}

void print(BigInt n) {
    printf("%s\n", to_string(n).c_str());
}

std::string to_string(BigInt n) {
    std::string str = "";
    BigInt num = BigInt(n);
    while (num != BigInt()) {
        BigInt next = num / 10;
        str = std::to_string((unsigned char)*(num - next * 10)[0]) + str;
        num = next;
    }
    if (str == "")
        return "0";
    return str;
}