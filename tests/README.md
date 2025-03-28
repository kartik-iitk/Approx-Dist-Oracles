1. `graph.txt`: On each line, `vertex1, vertex2, edge_weight`
2. `parameter.txt`: On each line,
   n # Number of vertices
   k # For the algorithm
   test_mode # To set if landmarks are chosen randomly or input by the user.
3. `queries.txt`: On each line, `vertex1, vertex2`

In the `./out` directory,

1. `landmarks.txt`: On each line, the number of vertices at that level followed by all the vertices which are landmarks, all space-separated.
2. `answers.txt`: The true distance (not approximate) for each query on a new line.
