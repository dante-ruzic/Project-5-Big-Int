#include "bigint.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
#include <stdexcept>


static int hexCharToVal(char c)  
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}


static char valToHexChar(uint8_t v)  
{
    return (v < 10) ? static_cast<char>('0' + v) : static_cast<char>('a' + (v - 10));
}

void bigint::initialize()
{
    for (int i = 0; i < CAPACITY; ++i)
    {
        numArray[i] = 0;
    }
}

bigint::bigint() 
{
    initialize();
}

bigint::bigint(uint64_t numbers) 
{
    initialize();
    int i = 0;

    while (numbers > 0 && i < CAPACITY)  
    {
        numArray[i] = static_cast<uint8_t>(numbers & 0xFF);
        numbers >>= 8;
        i++;
    }
}

bool bigint::setFromHex(const char characters[]) 
{
    initialize();
    if (characters == nullptr)
    {
        return false;
    }


    int length = 0;
    while (characters[length] != '\0')
        {
        if (hexCharToVal(characters[length]) < 0)
        {
            return false;
        }
        length++;
    }


    if (length == 0) 
    {
        return false;
    }


    int firstNonZero = 0;
    while (firstNonZero < length && characters[firstNonZero] == '0') 
    {
        firstNonZero++;
    }


    if (firstNonZero == length)  
    {
        return true;
    }


    int sigLength = length - firstNonZero;
    if (sigLength > MAX_HEX_CHARS)
    {
        return false;
    }


    int temp = 0;
    for (int i = length - 1; i >= firstNonZero; i -= 2)  
    {
        uint8_t lowNibble = static_cast<uint8_t>(hexCharToVal(characters[i]));
        uint8_t highNibble = 0;
        if (i - 1 >= firstNonZero)
        {
            highNibble = static_cast<uint8_t>(hexCharToVal(characters[i - 1]));
        }
        if (temp < CAPACITY) 
        {
            numArray[temp] = static_cast<uint8_t>((highNibble << 4) | lowNibble);
            temp++;
        }
    }
    return true;
}


bigint::bigint(const char characters[])  
{
    if (!setFromHex(characters)) 
    {
        throw std::invalid_argument("Invalid or overflowing hexadecimal string.");
    }
}


void bigint::debugPrint(std::ostream& out) const 
{

    for (int i = CAPACITY - 1; i >= 0; i--)  
    {
        out << valToHexChar(numArray[i] >> 4)
            << valToHexChar(numArray[i] & 0x0F) << '|';
    }
}


std::string bigint::toHexString() const  
{
    int i = CAPACITY - 1;

    while (i > 0 && numArray[i] == 0)
    {
        i--;
    }


    if (i == 0 && numArray[0] == 0)  
    {
        return "0";
    }

    std::string outStr;
    outStr.reserve((i + 1) * 2);


    uint8_t msb = numArray[i];
    if ((msb >> 4) != 0) 
    {
        outStr.push_back(valToHexChar(msb >> 4));
    }
    outStr.push_back(valToHexChar(msb & 0x0F));
    i--;


    while (i >= 0)
        {
        uint8_t b = numArray[i];
        outStr.push_back(valToHexChar(b >> 4));
        outStr.push_back(valToHexChar(b & 0x0F));
        i--;
    }

    return outStr;
}


std::ostream& operator<<(std::ostream& out, const bigint& obj)
{
    out << obj.toHexString();
    return out;
}


std::istream& operator>>(std::istream& in, bigint& obj)  
{
    char temp[MAX_HEX_CHARS + 2];
    char temp2[MAX_HEX_CHARS + 2] = {'\0'};
    int end = 0;

    while (in >> temp)
    {
        bool semiColon = false;
        int length = 0;

        while (temp[length] != '\0' && length < MAX_HEX_CHARS + 1)
        {
            length++;
        }

        for (int i = 0; i < length; i++) 
        {
            if (temp[i] == ';') 
            {
                semiColon = true;
                temp[i] = '\0';
                break;
            } else if (end < MAX_HEX_CHARS + 1)  
            {
                temp2[end++] = temp[i];
            }
        }
        if (semiColon)
        {
            break;
        }
    }
    temp2[end] = '\0';
    obj = bigint(temp2);
    return in;
}


bool bigint::operator==(const bigint& obj) const 
{
    for (int i = 0; i < CAPACITY; i++)
    {
        if (numArray[i] != obj.numArray[i])  
        {
            return false;
        }
    }
    return true;
}


bigint bigint::operator+(const bigint& obj) const
{
    bigint temp;
    uint32_t carry = 0;
    for (int i = 0; i < CAPACITY; i++)
    {
        uint32_t sum = static_cast<uint32_t>(numArray[i])
                     + static_cast<uint32_t>(obj.numArray[i]) + carry;
        temp.numArray[i] = static_cast<uint8_t>(sum & 0xFF);
        carry = sum >> 8;
    }
    return temp;
}


bigint bigint::timesDigit(int x) const
{
    if (x < 0 || x > 255)
    {
        return bigint();
    }
    bigint temp;
    uint32_t carry = 0;
    for (int i = 0; i < CAPACITY; i++)
    {
        uint32_t prod = static_cast<uint32_t>(numArray[i]) * static_cast<uint32_t>(x) + carry;
        temp.numArray[i] = static_cast<uint8_t>(prod & 0xFF);
        carry = prod >> 8;
    }
    return temp;
}


bigint bigint::times10(int x) const  
{
    if (x < 0 || x >= CAPACITY)  
    {
        return bigint();
    }
    bigint temp;
    for (int i = CAPACITY - x - 1; i >= 0; i--)  
    {
        temp.numArray[i + x] = numArray[i];
    }
    return temp;
}


bool bigint::multiplyChecked(bigint& res, const bigint& a, const bigint& b)  
{
    bigint temp;


    int lenA = CAPACITY;
    while (lenA > 0 && a.numArray[lenA - 1] == 0)
    {
        lenA--;
    }

    int lenB = CAPACITY;
    while (lenB > 0 && b.numArray[lenB - 1] == 0)
    {
        lenB--;
    }


    if (lenA == 0 || lenB == 0)  
    {
        res = temp;
        return true;
    }


    if (lenA + lenB - 1 > CAPACITY)  
    {
        return false;
    }


    for (int i = 0; i < lenA; i++)
        {
        if (a.numArray[i] == 0)  
        {
            continue;
        }

        uint32_t carry = 0;
        for (int j = 0; j < lenB; j++)
            {
            if (i + j >= CAPACITY)
                {
                if (b.numArray[j] != 0 || carry != 0)
                {
                    return false;
                }
                continue;
            }
            uint32_t prod = static_cast<uint32_t>(a.numArray[i])
                          * static_cast<uint32_t>(b.numArray[j])
                          + static_cast<uint32_t>(temp.numArray[i + j])
                          + carry;
            temp.numArray[i + j] = static_cast<uint8_t>(prod & 0xFF);
            carry = prod >> 8;
        }


        int dest = i + lenB;
        while (carry > 0)
        {
            if (dest >= CAPACITY)
            {
                return false;
            }
            uint32_t sum = static_cast<uint32_t>(temp.numArray[dest]) + carry;
            temp.numArray[dest] = static_cast<uint8_t>(sum & 0xFF);
            carry = sum >> 8;
            dest++;
        }
    }

    res = temp;
    return true;
}


bigint bigint::operator*(const bigint& obj) const
{
    bigint temp;
    if (!multiplyChecked(temp, *this, obj))  
    {
        throw std::overflow_error("Big integer multiplication overflow (>2048 bits).");
    }
    return temp;
}


int bigint::operator[](int i) const  
{
    if (i < 0 || i > CAPACITY - 1)
    {
        return 0;
    }
    return numArray[i];
}


bigint bigint::factorial(int x)  
{
    bigint obj(1);
    for (int i = 1; i < x; i++)  
    {
        obj = obj + (obj * bigint(static_cast<uint64_t>(i)));
    }
    return obj;
}


bigint bigint_init(const std::string& hexStr)
{
    return bigint(hexStr.c_str());
}

bigint bigint_init(const char* hexStr)
    {
    if (hexStr == nullptr)
    {
        throw std::invalid_argument("Null pointer passed to bigint_init.");
    }
    return bigint(hexStr);
}

bool bigint_init(bigint* out, const char* hexStr)
{
    if (out == nullptr || hexStr == nullptr) 
    {
        return false;
    }
    return out->setFromHex(hexStr);
}

std::string bigint_to_string(const bigint& obj)  
{
    return obj.toHexString();
}

bigint bigint_multiply(const bigint& a, const bigint& b) 
{
    return a * b;
}

bool bigint_multiply(bigint* res, const bigint* a, const bigint* b)  
{
    if (res == nullptr || a == nullptr || b == nullptr)  
    {
        return false;
    }
    return bigint::multiplyChecked(*res, *a, *b);
}