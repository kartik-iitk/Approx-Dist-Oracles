#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <limits>
#include <vector>

const double INF = std::numeric_limits<double>::infinity();

class Graph {
   public:
    int n;
    std::vector<std::vector<std::pair<int, double>>> adj;
    std::vector<std::vector<double>> trueDist;

    Graph(int n);
    void addBothEdges(int u, int v, double w);
    void addSingleEdge(int u, int v, double w);
    void allPairsShortest();
};

#endif  // GRAPH_H
