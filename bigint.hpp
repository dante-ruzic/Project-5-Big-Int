#ifndef bigint_HPP
#define bigint_HPP

#include <iostream>
#include <string>
#include <cstdint>
#include <cstddef>


const int CAPACITY = 256;
const int MAX_HEX_CHARS = CAPACITY * 2; 

class bigint 
{
public:
    void initialize();

    bigint();

    bigint(uint64_t numbers);

    bigint(const char characters[]);

    void debugPrint(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream&, const bigint&);

    bool operator==(const bigint&) const;

    friend std::istream& operator>>(std::istream&, bigint&);

    bigint operator+(const bigint&) const;

    bigint timesDigit(int) const;

    bigint times10(int) const;

    bigint operator*(const bigint&) const;

    int operator[](int) const;

    bigint factorial(int x);

    // Core helpers for Problem 1 and Problem 2
    bool setFromHex(const char characters[]);
    std::string toHexString() const;
    static bool multiplyChecked(bigint& res, const bigint& a, const bigint& b);

private:
    uint8_t numArray[CAPACITY];
};

// Problem 1 required functions
bigint bigint_init(const std::string& hexStr);
bigint bigint_init(const char* hexStr);
bool bigint_init(bigint* out, const char* hexStr);

std::string bigint_to_string(const bigint& obj);

// Problem 2 required functions
bigint bigint_multiply(const bigint& a, const bigint& b);
bool bigint_multiply(bigint* res, const bigint* a, const bigint* b);

#endif
