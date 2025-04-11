# Approximate Distance Oracles

## CS648 Mini Project 2024-25-II

## Group 4

| Name                  | Roll No. |
| --------------------- | -------- |
| Kartik Anant Kulkarni | 210493   |
| Siddharth Garg        | 211031   |
| Goural                | 210393   |

## Requirements

- CMake Version >= 3.10
- G++ Version 6.3.0 **OR** Apple clang version 16.0.0
- Python 3.11.10; External packages: Jupyter, Matplotlib, fibonacci-heap-mod

## How to compile

```bash
rm -rf build
mkdir build && cd build
cmake ..
make
```

This will generate an executable approx_dist_oracle

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

3. For stretch analysis, next run the executable to get true distances.

```bash
cd build
./true_distance <path/to/test/case/folder>
```

Eg:

```bash
./true_distance ../tests/test3
```

4. Use `./src/analysis.ipynb` for analysis and plots.

## References

1. https://github.com/renzibei/fph-table/tree/master
