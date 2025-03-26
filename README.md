# Approx-Dist-Oracles

## CS648 Mini Project 2024-25-II

## Group 4

| Name                  | Roll No. |
| --------------------- | -------- |
| Kartik Anant Kulkarni | 210493   |
| Siddharth Garg        | 211031   |
| Goural                | 210393   |

## Requirements

- CMake Version > 3.10
- G++ Version 6.3.0 **OR** Apple clang version 16.0.0

## How to compile

```bash
cd build
cmake ..
make
```

This will generate an executable approx_dist_oracle

## How to run

From the main directory, for running normally,

```bash
cd build
./approx_dist_oracle <parameter_file.txt> <graph.txt> <queries.txt>
```

## Implementation Phases

1. Raw algorithm and Test Cases
2. Path Similarity Comparison
3. Convert to Las Vegas.
5. Design Test cases:
   - Geomaps
   - Social Networks
   - Wireless Networks
6. Insights on multiple graphs
   - Graph Size (Scalability)
   - Density
   - Effect of k
   - Weighted v/s Unweighted
   -
