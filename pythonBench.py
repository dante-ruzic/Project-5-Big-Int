import os
import random
import subprocess
import sys
import time

BINARY = ".\\bigintTest.exe" if os.name == "nt" else "./bigintTest"
MAX_2048 = (1 << 2048) - 1


def run_cpp(args):
    proc = subprocess.run([BINARY] + args, capture_output=True, text=True, check=True)
    return proc.stdout.strip()


def run_python_verification():
    print("\n--- Running Python Cross-Verification Tests (Problem 3) ---")
    random.seed(42)

 
    for bits in [64, 256, 512, 1024]:
        for _ in range(25):
            a = random.getrandbits(bits)
            b = random.getrandbits(bits)
            hex_a = format(a, 'x')
            hex_b = format(b, 'x')

            expected = format(a * b, 'x') if (a * b) <= MAX_2048 else "ERROR"
            actual = run_cpp(["mul", hex_a, hex_b])
            if actual != expected:
                raise AssertionError(
                    f"Mismatch at {bits} bits:\n  a={hex_a}\n  b={hex_b}\n  got={actual}\n  expected={expected}"
                )
        print(f"[PASS] Python cross-verification for 25 random {bits}-bit multiplications.")


    for _ in range(10):
        a = random.getrandbits(1024) | (1 << 1023)
        b = random.getrandbits(1026) | (1 << 1025)
        assert a * b > MAX_2048
        actual = run_cpp(["mul", format(a, 'x'), format(b, 'x')])
        if actual != "ERROR":
            raise AssertionError("Expected ERROR on >2048-bit product")
    print("[PASS] Python cross-verification for >2048-bit overflow rejection.")
    print("All Python cross-verification tests passed successfully!")


def run_performance_evaluation():
    print("\n--- Problem 3: Performance Evaluation (C++ -O2 vs Python int) ---")
    iterations = 100000
    random.seed(2026)

    print(f"Iterations per operand size: {iterations} (timing multiplication ONLY)\n")
    print(f"{'Operand Size':<15} | {'C++ bigint (ns)':<18} | {'Python int (ns)':<18} | {'Ratio (C++/Py)':<15}")
    print("-" * 74)

    for bits in [256, 512, 1024]:
  
        a = random.getrandbits(bits) | (1 << (bits - 1))
        b = random.getrandbits(bits) | (1 << (bits - 1))
        hex_a = format(a, 'x')
        hex_b = format(b, 'x')

     
        cpp_out = run_cpp(["bench", str(iterations), hex_a, hex_b])
        cpp_ns = float(cpp_out)

        for _ in range(500):
            _ = a * b

        t0 = time.perf_counter_ns()
        for _ in range(iterations):
            _ = a * b
        t1 = time.perf_counter_ns()
        py_ns = (t1 - t0) / iterations

        ratio = cpp_ns / py_ns if py_ns > 0 else 0.0
        print(f"{f'{bits} bits':<15} | {cpp_ns:<18.2f} | {py_ns:<18.2f} | {ratio:.2f}x")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        run_python_verification()
    elif len(sys.argv) > 1 and sys.argv[1] == "bench":
        run_performance_evaluation()
    else:
        run_python_verification()
        run_performance_evaluation()