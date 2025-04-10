#ifndef APPROXDISTANCEORACLES_H
#define APPROXDISTANCEORACLES_H

#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_map>
#include <vector>

#include "Benchmark.h"
#include "Graph.h"
#include "fph/meta_fph_table.h"

class ApproxDistanceOracles {
   public:
    Graph* graph;
    int k;
    bool spaceopt;  // Rerun for Space Optimisation Enablement.
    bool debug;     // For Custom Landmarks

    std::vector<std::vector<int>> landmarks;
    std::vector<int> rank;
    std::vector<std::vector<int>> focus;
    std::vector<std::vector<double>> focus_distance;
    std::vector<std::vector<std::queue<std::pair<int, double>>>>
        ball;  // Level-wise
    std::vector<fph::MetaFphMap<int, double, fph::meta::MixSeedHash<int>>>
        ball_map;

    ApproxDistanceOracles(Graph* graph, int k, bool sp, bool dbg);
    void updateRank();
    void chooseLandmarks();
    void printLandmarks();
    void multiSourceDijkstra(const std::vector<int>& sources, int level);
    void printFocii();
    void trimmedDijkstra(int v, int level);
    void printBalls();
    long long hashBalls();
    std::pair<long long, long long> preprocess(
        const std::vector<std::vector<int>>& cust_land);
    double query(int u, int v);
};

#endif  // APPROXDISTANCEORACLES_H
