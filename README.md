# Merge Sort vs Multithreaded Merge Sort

LP-III : Design & Analysis of Algorithms (410241) - Mini Project
Topic 2: Merge Sort & Multithreaded Merge Sort

## Overview

This project implements and compares two versions of Merge Sort in C++:

1. **Normal Merge Sort** - classic single-threaded divide-and-conquer sort (also counts comparisons).
2. **Multithreaded Merge Sort** - uses `std::thread` to sort the left half in a new thread while the
   current thread sorts the right half. Thread creation is depth-limited (depth 3 = up to 8 threads)
   and sub-arrays smaller than `THRESHOLD` (1000) are sorted without spawning threads.

Both versions are run on random, best-case (sorted), reverse-sorted and worst-case inputs.

## Build & Run

```bash
g++ -O2 -std=c++17 -pthread merge_sort_comparison.cpp -o merge_sort_comparison
./merge_sort_comparison
```

Menu options:

- `1` - enter your own array; both versions sort it and the results, time and speed-up are printed.
- `2` - run the full performance test for n = 1,000 to 5,000,000 and thread counts 1 to 16.

## Results

Sample run on a 12-core CPU (full output in [benchmark_output.txt](benchmark_output.txt)):

| n         | Input  | Normal (ms) | Threaded (ms) | Speed-up |
|-----------|--------|-------------|---------------|----------|
| 1,000     | Random | 0.286       | 0.299         | 0.96     |
| 100,000   | Random | 12.442      | 4.487         | 2.77     |
| 1,000,000 | Random | 145.239     | 39.850        | 3.64     |
| 5,000,000 | Random | 790.968     | 219.307       | 3.61     |

- For small arrays the overhead of creating threads outweighs the benefit.
- For large arrays the multithreaded version is about 3.3x - 4.4x faster.
- 8 threads gave the best time; 16 threads were slower due to oversubscription and overhead.

## Complexity

| Version       | Time                       | Space |
|---------------|----------------------------|-------|
| Normal        | O(n log n)                 | O(n)  |
| Multithreaded | O(n log n) work, O(n) span | O(n)  |

## Repository Structure

```
merge_sort_comparison.cpp            Source code (both versions + benchmark)
benchmark_output.txt                 Sample benchmark output
figures/                             Flowcharts, merge tree, time and speed-up charts
Merge_Sort_Mini_Project_Report.pdf   Project report (PDF)
Merge_Sort_Mini_Project_Report.docx  Project report (Word)
```
