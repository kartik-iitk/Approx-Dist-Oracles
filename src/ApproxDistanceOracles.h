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
    long long k;
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

    ApproxDistanceOracles(Graph* graph, long long k, bool sp, bool dbg);
    void updateRank();
    void chooseLandmarks();
    void printLandmarks();
    void multiSourceDijkstra(const std::vector<int>& sources, long long level);
    void printFocii();
    void trimmedDijkstra(long long v, long long level);
    void printBalls();
    long long hashBalls();
    std::pair<long long, long long> preprocess(
        const std::vector<std::vector<int>>& cust_land);
    double query(long long u, long long v);
};

#endif  // APPROXDISTANCEORACLES_H
