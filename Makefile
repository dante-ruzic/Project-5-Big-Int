CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
PYTHON   = py

ifeq ($(OS),Windows_NT)
	RM = cmd /C del /Q /F
else
	RM = rm -f
endif

.PHONY: all test tests demo bench clean

all: test demo

bigintTest: bigint.cpp bigintTest.cpp bigint.hpp
	$(CXX) $(CXXFLAGS) bigint.cpp bigintTest.cpp -o bigintTest

test: bigintTest
	./bigintTest test
	$(PYTHON) pythonBench.py test

tests: test

demo: bigintTest
	./bigintTest demo
	$(PYTHON) pythonBench.py bench

bench: demo

clean:
	$(RM) bigintTest.exe bigintTest *.o
