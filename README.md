# Approximate Distance Oracles

## CS648 Mini Project 2024-25-II

This project presents an efficient implementation of approximate distance oracles. The solution features a modular, multi-executable design, enabling flexible experimentation and evaluation of the core algorithm of [Thorup-Zwick](https://doi.org/10.1145/1044731.1044732). We have also implemented the optimized variant of Dijkstra's algorithm and used a perfect hashing library. The implementation is tailored for clarity and extensibility, supporting comprehensive analysis and visualization of results. The `./Report.pdf` outlines an enhanced description of the algorithm with intuition and rigorous proof.

## Requirements

- CMake Version >= 3.10
- Apple Clang version 16.0.0 **OR** G++ Version 6.3.0 (Not fully tested)
- Python 3.11.10; External packages: Jupyter, Matplotlib

## How to compile

```bash
rm -rf build
mkdir build && cd build
cmake ..
make
```

This will generate two executables `./build/approx_dist_oracle` and `./build/true_distance`.

## How to run

From the repository directory.

1. Prepare the test-case folder using instructions mentioned in `./tests/README.md`
2. Run the approximate distance oracles to get the output of all the queries using;

```bash
cd build
./approx_dist_oracle <path/to/test/case/folder>
```

Eg:

```bash
./approx_dist_oracle ../tests/test3
```

3. For stretch analysis, next run the executable to get true distances. The use_dijkstra boolean should be 1 if exact Dijkstra is to be used else 0 to use all pairs shortest paths.

```bash
cd build
./true_distance <path/to/test/case/folder> <use_dijkstra>
```

Eg:

```bash
./true_distance ../tests/test3 1
```

4. Use `./src/analysis.ipynb` for analysis and plots.

## Group 4

| Name                  | Roll No. |
| --------------------- | -------- |
| Kartik Anant Kulkarni | 210493   |
| Siddharth Garg        | 211031   |
| Goural                | 210393   |
