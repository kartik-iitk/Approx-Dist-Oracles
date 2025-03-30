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

1. Manually Generated Test Cases which, for a given graph and chosen landmarks, checks if the approximate distance reported by the code is correct. Used for testing is our logic is correct. Try for a suffciently large graph, a good number of test cases. Add preprocess.py for the graph inputs.

SYNTHETIC TEST CASES

2. Generate Graphs using (https://github.com/eXascaleInfolab/LFR-Benchmark_UndirWeightOvp) which will vary 1 hyperparameter at a time. Key hyperparameters of interest:
   1. Graph Size - n = 10^2, 10^3, 10^4, 10^5, 10^7, 10^8 (Try to get a graph of ~500 MB or so).
   2. Max Degree - Decide range and scale
   3. Avg Degree - Decide range and scale
   4. Effect of k - Choose k from 2, 3, 4, 5, 6,…, log(n)
      Observe the effect of varying these by creating separate folder for each test case. Further note down in README.md the individual parameters for each test case (so that we can create a table).
3. For small n (<= 10^3), run all the O(n^2) queries, but for larger, choose 100n random queries or so. Key Output variables of interest:
   1. preprocessing time - benchmarking
   2. % Error in output distance (when compared to the true distance).
   3. path similarity metrics - for closeby points, faraway points, report how going via w, affects the vertices being chosen in the shortest path. This will offer insight into why this algorithm is efficient.
4. Make Plots and Report Analysis

REAL-WORLD TEST CASES

5. Create new folders for the downloaded test cases and update the README of the testcase folder with appropriate details. Include all types of graphs:
   - See for examples https://ae.iti.kit.edu/documents/theses/An_Approximate_Distance_Oracle_for_Social_Networks.pdf
   - Perform Weighted versus Unweighted Comparison (in case there is an example where we have a weighted undirected graph).
   - Cover all kinds of examples (try for huge graph): Geomaps, social networks, wireless networks etc.
   - Again Note Down:
     - Preprocessing time
     - % Error
     - Path Similarity
6. Make plots and analyse

LAS VEGAS

1. Total Ball Size computation and corresponding reruns (add info to log as necessary).
   (Note this will solve problem if any landmark level has all vertices or is fully empty).
