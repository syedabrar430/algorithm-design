# COT5405 – Project 1: Greedy and Divide-and-Conquer Algorithms

**Authors:** Abrar Syed and Estefania Rodriguez Da Silva

## Project Description

This project implements and tests two scheduling algorithms in C++:

1. **Greedy Algorithm – Johnson's Rule (`greedy.cpp`):** Finds an optimal order for laundry loads that must be processed by one washer and one dryer, minimizing the total completion time (makespan).

2. **Divide-and-Conquer Algorithm – Round-Robin Scheduling (`divideandconquer.cpp`):** Generates a round-robin schedule for \(n = 2^k\) teams, where each team plays every other team exactly once in the minimum number of rounds.

Both programs include correctness checks and runtime experiments to compare their performance with the theoretical time complexities.

## How to Compile and Run

Open a terminal in the folder containing the `greedy.cpp` and `divideandconquer.cpp` files.

### 1. Greedy Algorithm

Compile and run using:

```bash
g++ greedy.cpp -o greedy
./greedy
```

Alternatively, to specify C++17 and optimization, use:

```bash
g++ -O2 -std=c++17 greedy.cpp -o greedy && ./greedy
```

The expected correctness message that should be outputted is:

```text
Johnson matches brute force on 2000 random instances
```

The program checks Johnson's rule against brute-force solutions for 2,000 small random instances and then measures the runtime for different input sizes and generates 2 CSV files.

**Generated CSV files:**
- `greedy_runtime.csv` – Contains input sizes, measured execution times, normalized runtimes, and reference curves.
- `greedy_quality.csv` – Compares Johnson's rule with randomly generated schedules using a makespan lower bound.

### 2. Divide-and-Conquer Algorithm

Compile and run using:

```bash
g++ divideandconquer.cpp -o divideandconquer
./divideandconquer
```

Alternatively, to specify C++17 and optimization, use:

```bash
g++ -O2 -std=c++17 divideandconquer.cpp -o divideandconquer && ./divideandconquer
```
The program validates the generates round-robin schedules for powers-of-two team counts from 2 through 512. It then measures the schedule construction time for input sizes from 16 through 8192 teams.

**Generated file:**
- `dc_runtime.csv` – Contains input sizes, measured execution times, normalized runtimes, and the quadratic reference curve.

The expected correctness message is:

```text
Schedule valid for n = 2 .. 512
```

## Experimental Results

The greedy algorithm has a theoretical running time of **O(n log n)** because it sorts the laundry loads.

The divide-and-conquer algorithm has a theoretical running time of **Θ(n²)** because it generates all the matches in a complete round-robin schedule.

Both programs automatically run their correctness tests and runtime experiments when executed. The resulting CSV files are saved in the current working directory and can be used to reproduce the tables and graphs in our report.

Execution times may vary depending on the computer, compiler, and system activity. Our experiments were run using Apple Clang 21.0.0 with C++17 and `-O2` optimization.
