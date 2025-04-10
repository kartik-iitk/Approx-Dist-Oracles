# Testing Data Format

## Creating a New Testcase

1. Put the edges file as `./test<number>/inp/graph.txt`. Ensure the edge weights in order (u, v, w) format
   Note: u, v can be 0 or 1 indexed and needs to be configured later.

2. Write the input parameters for running the code in the order as given below.

3. Generate queries for the testcase using `../src/querygen.py`. Choose the output-file name as `./test<number>/inp/queries.txt`. Eg: When running from the build directory, use:

   ```bash
   python ../src/querygen.py -t ../tests/test1 -m uniform -q 12
   ```

**Note**: For next steps, on how to run both the executables next, see the main directory's `README.md`.

## Testcase Folder Organisation

Each test-case folder will have the following structure:

```
test<num>/
├── inp/
│   ├── graph.txt       # Input graph edges file
│   ├── queries.txt     # Generated queries file
│   ├── landmarks.txt   # Custom landmarks file (Optional)
│   └── parameter.txt   # Input parameters for the test case
└── out/
    ├── answers.txt     # Returned Answers to the queries
    ├── distance.txt    # Correct Result of Query Answers
    ├── log.txt         # Has Error Stream output for the main executable
    └── runstats.txt    # Has average timings and other details.
```

## Specific File Details

- `graph.txt`: On each line, `vertex1, vertex2, edge_weight`

- `queries.txt`: On each line, `vertex1, vertex2`

- `landmarks.txt`: On each line, first write the number of vertices at that level followed by the index of all vertices which are landmarks of that level, all space-separated. Follow the convention (0-indexed or 1-indexed of the input file)

- `parameters.txt`: On every new line,

| No  | Parameter     | Description                                                                   |
| --- | ------------- | ----------------------------------------------------------------------------- |
| 1   | `n`           | Number of vertices                                                            |
| 2   | `k`           | For the algorithm                                                             |
| 3   | `num_threads` | Max number of threads to be run in parallel                                   |
| 4   | `g`           | `1` if the undirected graph is 1-indexed                                      |
| 5   | `both`        | `1` if `graph.txt` has entries for both `(u, v, w)` and `(v, u, w)`           |
| 6   | `spaceopt`    | `1` will rerun preprocessing until a suitable size is achieved                |
| 7   | `verbose`     | `1` will print all the balls, landmarks and focii to `log.txt`                |
| 8   | `debug`       | `1` means the code will use the custom landmarks mentioned in `landmarks.txt` |

- `answers.txt`: The approximate distance returned for each query, by the algorithm.

- `distance.txt`: The exact distance between the nodes for each query, used to calculate stretch.

- `log.txt`: Error Stream output for the main executable execution. It will also contain any printing statement executed.

- `runstats.txt`: All the necessary stats for the code, summarised:
  - Next integer here will store the number of preprocess calls that took place. Note: This maybe more than one in case spaceopt is enabled.
  - Next value is the miliseconds taken by the preprocess function (combined for all calls).
  - Next Value is the size of the datastructure in words (counting pair of d).
  - All the next values store the query time in nano-seconds for all queries in queries.txt, each on a new line.
