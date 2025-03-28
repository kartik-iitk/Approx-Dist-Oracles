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

## How to compile

```bash
rm -rf build
mkdir build && cd build
cmake ..
make
```

This will generate an executable approx_dist_oracle

## How to run

From the main directory, for running normally,

```bash
cd build
./approx_dist_oracle <parameter_file.txt> <graph_file.txt> <queries_file.txt>
```

For test runs, set appropriate parameters and update the landmarks and run:

```bash
cd build
./approx_dist_oracle <parameter_file.txt> <graph_file.txt> <queries_file.txt> <landmarks_file.txt> <answers_file.txt>
```

Eg:

```bash
./approx_dist_oracle ../tests/test3/parameters.txt ../tests/test3/graph.txt ../tests/test3/queries.txt ../tests/test3/out/landmarks.txt ../tests/test3/out/answers.txt
```

## Implementation Phases

1. Raw algorithm and Test Cases
2. Path Similarity Comparison
3. Convert to Las Vegas.
4. Design Test cases:
   - Geomaps
   - Social Networks
   - Wireless Networks
5. Insights on multiple graphs
   - Graph Size (Scalability)
   - Density
   - Effect of k
   - Weighted v/s Unweighted
   -
