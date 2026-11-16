# Assignment 5: Big Integers

Name:
- Dante Ruzic

Language Used:
- C++
- Python 3

Build/Test:
- Makefile is used by running:

    C:\Users[user]\Documents\GitHub\Big-Integers> make tests(test, demo, bench, all, tests)

For automated testing and performance evaluation.

    C:\Users[user]\Documents\GitHub\big int assignment 5> g++ -std=c++17 -O2 -Wall -Wextra bigint.cpp bigintTest.cpp -o bigintTest
    C:\Users[user]\Documents\GitHub\big int assignment 5> .\bigintTest test
    C:\Users[user]\Documents\GitHub\big int assignment 5> py pythonBench.py test
    
To run the test files without makefile.

     C:\Users[user]\Documents\GitHub\big int assignment 5> g++ -std=c++17 -O2 -Wall -Wextra bigint.cpp bigintTest.cpp -o bigintTest
    C:\Users[user]\Documents\GitHub\big int assignment 5> .\bigintTest demo
    C:\Users[user]\Documents\GitHub\big int assignment 5> .\bigintTest init (hex)
    C:\Users[user]\Documents\GitHub\big int assignment 5> .\bigintTest mul (hexA) (hexB)
    C:\Users[user]\Documents\GitHub\big int assignment 5> .\bigintTest bench (iters) (hexA) (hexB)
    C:\Users[user]\Documents\GitHub\big int assignment 5> py pythonBench.py bench

To manually run each command in the command line: running the default demo, initializing/normalizing a hex string, multiplying two big integers, and running the 256/512/1024-bit performance benchmark.

Limitations:

- System used for this was Windows, therefore any examples or ways to run the code have only been tested on Powershell and Command Line.

Disclosure:

- I used A.I. to help with mathematical proof understanding if anything did not make sense, such as using Markdown for mathematics and tables.
- I verified the Markdown using the following site "https://markdownviewer.org/".
