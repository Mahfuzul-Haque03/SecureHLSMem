# secureHLSMem: Memory Safety for High-Level Synthesis Accelerators

**secureHLSMem** is a modular defense framework designed to detect and mitigate memory safety vulnerabilities in C/C++ kernels used for High-Level Synthesis (HLS). It targets the critical gap where standard HLS tools (like Vitis HLS) prioritize performance over safety, allowing dangerous "silent corruption" bugs to pass through simulation and synthesis into deployed hardware.

## Why Memory Safety Matters in HLS

HLS kernels are susceptible to the same low-level "foot-guns" as standard C code, but the consequences in hardware are often silent and catastrophic.

### 1. Out-of-Bounds (OOB) Write
**What it is:** Writing data outside the intended memory region. 
**Real-world Analogy:** Installing a jacuzzi in your neighbor's living room. The system won't like it.
**Danger:** Corrupts adjacent memory (registers, stack, other buffers), leading to silent data corruption or system instability.
**Example:**
```c
char buf[8];
for (int i = 0; i <= 8; i++) { // Off-by-one error
    buf[i] = 'A'; // Writes to buf[8] -> OOB
}
```

### 2. Out-of-Bounds (OOB) Read
**What it is:** Reading memory outside the allocated region.
**Real-world Analogy:** Peeking over your neighbor's fence. You aren't changing anything, but you're seeing things you shouldn't.
**Danger:** Leaks sensitive data (keys, weights) or enables side-channel attacks.
**Example:**
```c
char* p = buffer - 4; // Underflow
char secret = p[0];   // Reads unrelated/secret memory
```

### 3. NULL Pointer Dereference
**What it is:** Accessing memory through a pointer holding `0x0`.
**Real-world Analogy:** Dialing zero on a phone—nobody's home, and the call fails.
**Danger:** In software, this crashes the program. In HLS, it can lead to undefined behavior or deadlocked hardware states.
**Example:**
```c
int *x = NULL;
*x = 42; // Boom
```

---

## The Problem: "Silent Corruption" in Vitis HLS

Our experiments demonstrate that current industry-standard HLS flows offer **zero protection** against these bugs.

### Baseline Findings
We created a benchmark suite of 6 kernels containing seeded OOB and NULL errors.
1.  **Static Analysis**: `clang -Wall` and `cppcheck` achieved **0% Recall**. They analyze kernels in isolation and cannot see the size of pointers passed from the testbench.
2.  **Dynamic Simulation**: Vitis HLS `csim` **PASSED** for 5 out of 6 buggy kernels. The simulation simply corrupted neighbor variables on the stack without warning.
3.  **Synthesis**: Vitis HLS synthesized the buggy code with **no overhead**, treating the bugs as valid optimized logic. This confirms that **Vitis provides no defense mechanism** by default.

---

## The Solution: secureHLSMem

We propose and implement a modular defense mechanism with two core components:

1.  **Static Memory-Safety Analyzer**: A pre-synthesis pass that understands HLS patterns (tiling, sliding windows) to detect potential OOB/NULL hazards.
2.  **Hardware-Assisted Bounds Checking Unit (BCU)**: An instrumentation pass that injects synthesizable, lightweight runtime checks around memory accesses.

### Implemented Workflow
The `securehls` Python package automates the entire flow:

```bash
# 1. Static Check (First Line of Defense)
python3 securehls/cli.py check 

# 2. Instrument & Verify (Runtime Safety Net)
python3 securehls/cli.py instrument --mode bcu
```

---

## Experimental Results

We evaluated `secureHLSMem` on the benchmark suite.

### 1. Detection Capabilities
| Tool | Recall | Precision | Notes |
| :--- | :---: | :---: | :--- |
| **Baseline (Clang)** | 0% | N/A | Missed all interface-related bugs |
| **secureHLSMem Static** | **100%** | **100%** | Correctly flagged all 6 seeded bugs |

### 2. Mitigation Capabilities
| Kernel | Baseline csim | secureHLSMem csim | Mitigation Result |
| :--- | :---: | :---: | :--- |
| `fir_buggy` | PASS (Silent) | **FAIL (Assert)** | ✅ Prevented Corruption |
| `gemm_buggy` | PASS (Silent) | **FAIL (Assert)** | ✅ Prevented Corruption |
| `conv2d_buggy` | PASS (Silent) | **FAIL (Assert)** | ✅ Prevented Corruption |

### 3. Hardware Cost (Trade-off)
We synthesized the secure kernels to measure the overhead of the BCU logic.

| Kernel | Safe LUTs | Secured LUTs | Overhead |
| :--- | :---: | :---: | :---: |
| **FIR Filter** | 1016 | 1057 | **~4.0%** |
| **GEMM** | 996 | 1032 | **~3.6%** |
| **MaxPool** | 204 | 209 | **~2.5%** |

**Conclusion:** `secureHLSMem` provides complete safety coverage with negligible hardware cost (<5%).

---

## How to Use

1.  **Clone the Repository**:
    ```bash
    git clone https://github.com/your-repo/secureHLSMem.git
    cd secureHLSMem
    ```

2.  **Run Benchmarks**:
    ```bash
    # Run Static Analysis
    python3 securehls/cli.py check
    
    # Run Instrumentation and Vitis Verification
    python3 securehls/cli.py instrument --mode bcu
    python3 setup_vitis.py
    python3 run_vitis.py
    ```

## Citation

If you use `secureHLSMem` in your research, please cite:

> **Md Rubel Ahmed, M Shifat Hossain, et al.**, "secureHLSMem: A Modular Defense Framework for Memory Safety in HLS Accelerators," [Conference/Journal Name], 2025.

---

**Authors:** Md Rubel Ahmed, M Shifat Hossain
