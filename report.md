# Assignment 5: Big Integers
**Name:** Dante Ruzic

## Problem 1: Big Integer Representation
`bigint` represents non-negative integers up to 2048 bits using a fixed-size, little-endian array of 256 unsigned bytes (`uint8_t numArray[256]`), where index `0` is the least significant byte and index `255` is the most significant byte.

- **Justification:**
  1. **Stack Allocation:** Bounding the array at 256 bytes keeps all data on the stack and avoids dynamic memory allocation overhead.
  2. **$O(n)$ Hex Conversion:** Because 1 byte equals 2 hex digits, `bigint_init` and `bigint_to_string` pack and unpack hex characters directly using bitwise shifts and masks without multi-precision division.
- **Validation:** `bigint_init` strips leading zeros and rejects empty strings, non-hex characters, or inputs exceeding 512 significant hex digits ($>2048$ bits). `bigint_to_string` outputs lowercase hex with no leading zeros (or `"0"` for zero).

## Problem 2: Big Integer Multiplication
`bigint_multiply` implements textbook $O(n^2)$ schoolbook multiplication in base 256:
1. **Trim & Early Overflow Check:** Finds the active byte lengths `lenA` and `lenB`. Returns `0` if either is zero, or reports an overflow error if `lenA + lenB - 1 > 256`.
2. **Multiply-Accumulate:** Loops over $i \in [0, \text{lenA}-1]$ and $j \in [0, \text{lenB}-1]$, computing `prod = a[i] * b[j] + temp[i + j] + carry` inside a `uint32_t`, storing `prod & 0xFF` at `temp[i + j]`, and updating `carry = prod >> 8`.
3. **Overflow Detection:** Reports an error if any non-zero byte or carry reaches index $\ge 256$ ($>2048$ bits).

## Problem 3: Testing and Performance Evaluation

### Testing Results
- **C++ Unit Tests (`bigintTest.cpp`):** Passed all checks for byte layout (`ab54a98ceb1f0ad2`), case-insensitivity, leading zeros, invalid/overflowing inputs, 2048-bit boundary products, and 100 random $64 \times 64$-bit multiplications.
- **Python Cross-Verification (`pythonBench.py test`):** Passed 100 random multiplications across 64, 256, 512, and 1024 bits and 10 overflow cases ($>2048$ bits) verified directly against Python's `int(s, 16)` and `format(x, 'x')`.

### Performance Results
Compiled with `g++ -std=c++17 -O2` and averaged over 100,000 multiplications per operand size on identical inputs (timing multiplication only):

| Operand Size | C++ `bigint` (`-O2`) | Python `int` | Ratio (C++ / Python) |
| :--- | :--- | :--- | :--- |
| **256 bits** | 715.73 ns | 95.95 ns | 7.46x |
| **512 bits** | 2780.57 ns | 238.95 ns | 11.64x |
| **1024 bits** | 10729.51 ns | 810.35 ns | 13.24x |

### Explanation
1. **Quadratic Scaling:** Doubling the bit width doubles the active byte count $n$ ($32 \to 64 \to 128$ bytes) and quadruples the $n^2$ inner-loop operations, causing the C++ runtime to increase by $\approx 4\times$ at each step ($3.88\times$ and $3.86\times$).
2. **C++ vs. Python:** Python's `int` is written in C using 30-bit limbs rather than 8-bit bytes. For 1024-bit operands, C++ performs $128^2 = 16,384$ byte operations while Python needs only $\lceil 1024/30 \rceil^2 = 35^2 = 1,225$ limb operations ($13.37\times$ fewer), matching the $13.24\times$ runtime ratio. At 256 bits, the ratio is smaller ($7.46\times$) due to Python's per-object heap allocation and interpreter overhead.