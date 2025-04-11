#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <limits>
#include <vector>

const double INF = std::numeric_limits<double>::infinity();

class Graph {
   public:
    long long n;
    std::vector<std::vector<std::pair<int, double>>> adj;
    std::vector<std::vector<double>> trueDist;

    Graph(long long n);
    void addBothEdges(long long u, long long v, double w);
    void addSingleEdge(long long u, long long v, double w);
    void allPairsShortest();
    double dijkstra(long long u, long long v);
};

#endif  // GRAPH_H
