#include "bigint.hpp"
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

struct MulTestCase {
    const char* aHex;
    const char* bHex;
    const char* expectedHex;
};

void runTests() {
    std::cout << "--- Running Automated Big Integer Tests (Problems 1, 2, and 3) ---" << std::endl;


    bigint example = bigint_init("ab54a98ceb1f0ad2");
    const uint8_t expectedBytes[8] = {0xD2, 0x0A, 0x1F, 0xEB, 0x8C, 0xA9, 0x54, 0xAB};
    for (int i = 0; i < 8; i++) 
    {
        if (example[i] != expectedBytes[i]) 
        {
            throw std::runtime_error("Byte mismatch at index " + std::to_string(i));
        }
    }
    for (int i = 8; i < CAPACITY; i++) 
    {
        if (example[i] != 0x00) 
        {
            throw std::runtime_error("Expected zero byte at index " + std::to_string(i));
        }
    }
    if (bigint_to_string(example) != "ab54a98ceb1f0ad2") 
    {
        throw std::runtime_error("bigint_to_string failed on assignment example.");
    }
    std::cout << "[PASS] Problem 1 byte array layout and hex conversion for 12345678901234567890." << std::endl;


    if (bigint_to_string(bigint_init("0")) != "0" ||
        bigint_to_string(bigint_init("00000000000000000000")) != "0") 
        {
        throw std::runtime_error("Zero formatting failed.");
    }
    bigint mixed = bigint_init("00000000AB54A98CEB1F0AD2");
    if (!(mixed == example) || bigint_to_string(mixed) != "ab54a98ceb1f0ad2") 
    {
        throw std::runtime_error("Uppercase/leading-zero normalization failed.");
    }
    std::string longLeadingZeros = std::string(600, '0') + "deadbeef";
    if (bigint_to_string(bigint_init(longLeadingZeros)) != "deadbeef") 
    {
        throw std::runtime_error("Failed to allow >512 total chars when caused by leading zeros.");
    }
    std::cout << "[PASS] Problem 1 uppercase/lowercase, leading zeros, and zero formatting checks." << std::endl;


    const char* badInputs[] = {"", "0x1234", "123g45", "ab cd", "-1234"};
    for (const char* bad : badInputs) 
    {
        try 
        {
            bigint_init(bad);
            throw std::runtime_error(std::string("bigint_init should reject invalid input: ") + bad);
        } catch (const std::invalid_argument&) 
        {

        }
    }
    std::string overflowInput(513, '1'); 
    try 
    {
        bigint_init(overflowInput);
        throw std::runtime_error("bigint_init should reject 513-digit hex input (>2048 bits).");
    } catch (const std::invalid_argument&) 
    {

    }
    std::cout << "[PASS] Problem 1 invalid input rejection checks (empty, non-hex, >2048 bits)." << std::endl;


    MulTestCase knownTable[] = 
    {
        {"0", "ab54a98ceb1f0ad2", "0"},
        {"1", "ab54a98ceb1f0ad2", "ab54a98ceb1f0ad2"},
        {"ab54a98ceb1f0ad2", "ab54a98ceb1f0ad2", "72aa3681f6e120d4a3ff1b512b511444"},
        {"ffffffffffffffff", "ffffffffffffffff", "fffffffffffffffe0000000000000001"},
        {"10000000000000000", "10000000000000000", "100000000000000000000000000000000"}
    };
    for (const MulTestCase& tc : knownTable) 
    {
        bigint a = bigint_init(tc.aHex);
        bigint b = bigint_init(tc.bHex);
        bigint prod = bigint_multiply(a, b);
        if (bigint_to_string(prod) != tc.expectedHex) 
        {
            throw std::runtime_error(std::string("Product mismatch for ") + tc.aHex + " * " + tc.bHex);
        }
    }
    bigint small = bigint_init("0102");
    if (bigint_to_string(small.timesDigit(3)) != "306" ||
        bigint_to_string(small.times10(1)) != "10200") 
        {
        throw std::runtime_error("Class helper methods timesDigit/times10 failed.");
    }
    std::cout << "[PASS] Problem 2 known-answer multiplication table and class operator checks." << std::endl;

    std::string max2048Hex(512, 'f');
    bigint max2048 = bigint_init(max2048Hex);
    bigint one = bigint_init("1");
    bigint two = bigint_init("2");

    if (bigint_to_string(bigint_multiply(max2048, one)) != max2048Hex) 
    {
        throw std::runtime_error("Failed exact 2048-bit multiplication by 1.");
    }
    try 
    {
        bigint_multiply(max2048, two);
        throw std::runtime_error("bigint_multiply should throw on 2049-bit overflow (max * 2).");
    } catch (const std::overflow_error&) 
    {

    }

    std::string pow2_1024 = "1" + std::string(256, '0');
    bigint p1024 = bigint_init(pow2_1024);
    try 
    {
        bigint_multiply(p1024, p1024);
        throw std::runtime_error("bigint_multiply should throw on 2^1024 * 2^1024 (2049 bits).");
    } catch (const std::overflow_error&) 
    {

    }
    std::cout << "[PASS] Problem 2 2048-bit boundary and >2048-bit overflow rejection checks." << std::endl;


    std::mt19937_64 rng(12345);
    for (int trial = 0; trial < 100; trial++) 
    {
        uint64_t x = rng();
        uint64_t y = rng();
        __uint128_t exact = static_cast<__uint128_t>(x) * static_cast<__uint128_t>(y);

        char hexX[17], hexY[17];
        std::snprintf(hexX, sizeof(hexX), "%llx", static_cast<unsigned long long>(x));
        std::snprintf(hexY, sizeof(hexY), "%llx", static_cast<unsigned long long>(y));

        bigint bx = bigint_init(hexX);
        bigint by = bigint_init(hexY);
        bigint bxy = bigint_multiply(bx, by);
        bigint byx = bigint_multiply(by, bx);

        if (!(bxy == byx)) 
        {
            throw std::runtime_error("Commutativity check failed.");
        }

        uint64_t lo = static_cast<uint64_t>(exact);
        uint64_t hi = static_cast<uint64_t>(exact >> 64);
        char expectedBuf[33];
        if (hi > 0) 
        {
            std::snprintf(expectedBuf, sizeof(expectedBuf), "%llx%016llx",
                          static_cast<unsigned long long>(hi),
                          static_cast<unsigned long long>(lo));
        } else 
        {
            std::snprintf(expectedBuf, sizeof(expectedBuf), "%llx",
                          static_cast<unsigned long long>(lo));
        }
        if (bigint_to_string(bxy) != expectedBuf) 
        {
            throw std::runtime_error("Randomized 64x64 multiplication mismatch.");
        }
    }
    std::cout << "[PASS] Problem 3 randomized multiplication consistency checks (100 trials)." << std::endl;
    std::cout << "All automated C++ tests passed successfully!" << std::endl;
}

void runDemo() 
{
    std::cout << "--- Problem 1 & 2: Big Integer Representation and Multiplication Demo ---" << std::endl;
    const char* sampleHex = "ab54a98ceb1f0ad2";
    bigint a = bigint_init(sampleHex);
    std::cout << "Input Hex           : " << sampleHex << " (12345678901234567890)" << std::endl;
    std::cout << "Little-Endian Bytes : [";
    for (int i = 0; i < 8; i++) 
    {
        std::cout << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << a[i];
        if (i < 7) std::cout << ", ";
    }
    std::cout << std::dec << "]" << std::endl;
    std::cout << "bigint_to_string(a) : " << bigint_to_string(a) << std::endl;

    bigint prod = bigint_multiply(a, a);
    std::cout << "a * a (hex)         : " << bigint_to_string(prod) << std::endl;
}

int main(int argc, char* argv[]) 
{
    try 
    {
        if (argc == 1 || (argc == 2 && std::strcmp(argv[1], "test") == 0)) 
        {
            runTests();
        } else if (argc == 2 && std::strcmp(argv[1], "demo") == 0) 
        {
            runDemo();
        } else if (argc == 3 && std::strcmp(argv[1], "init") == 0) 
        {
            bigint a;
            if (!bigint_init(&a, argv[2])) 
            {
                std::cout << "ERROR" << std::endl;
            } else 
            {
                std::cout << bigint_to_string(a) << std::endl;
            }
        } else if (argc == 4 && std::strcmp(argv[1], "mul") == 0) 
        {
            bigint a, b, res;
            if (!bigint_init(&a, argv[2]) || !bigint_init(&b, argv[3]) || !bigint_multiply(&res, &a, &b)) 
            {
                std::cout << "ERROR" << std::endl;
            } else 
            {
                std::cout << bigint_to_string(res) << std::endl;
            }
        } else if (argc == 5 && std::strcmp(argv[1], "bench") == 0) 
        {
            long iterations = std::strtol(argv[2], nullptr, 10);
            bigint a, b, res;
            if (!bigint_init(&a, argv[3]) || !bigint_init(&b, argv[4])) 
            {
                std::cerr << "Invalid benchmark operands." << std::endl;
                return 1;
            }


            for (int i = 0; i < 500; i++) 
            {
                bigint_multiply(&res, &a, &b);
            }


            volatile uint8_t sink = 0;
            auto start = std::chrono::steady_clock::now();
            for (long i = 0; i < iterations; i++) 
            {
                bigint_multiply(&res, &a, &b);
                sink ^= static_cast<uint8_t>(res[0]);
            }
            auto end = std::chrono::steady_clock::now();
            (void)sink;

            double totalNs = std::chrono::duration<double, std::nano>(end - start).count();
            std::cout << std::fixed << std::setprecision(2) << (totalNs / iterations) << std::endl;
        } else 
        {
            std::cout << "Usage:" << std::endl;
            std::cout << "  bigintTest test                      (Runs automated unit tests)" << std::endl;
            std::cout << "  bigintTest demo                      (Runs default assignment demo)" << std::endl;
            std::cout << "  bigintTest init <hex>                (Validates and normalizes hex)" << std::endl;
            std::cout << "  bigintTest mul <hexA> <hexB>         (Multiplies two hex bigints)" << std::endl;
            std::cout << "  bigintTest bench <iters> <hexA> <hexB>" << std::endl;
        }
    } catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}