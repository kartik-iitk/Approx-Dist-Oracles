#include "Graph.h"

Graph::Graph(int n) : n(n) {
    if (n <= 0) {
        std::cerr << "Invalid number of vertices: " << n << std::endl;
        return;
    }
    adj.resize(n);
    trueDist.resize(n, std::vector<double>(n, INF));
}

void Graph::addBothEdges(int u, int v, double w) {
    if (u < 0 || v < 0 || u >= n || v >= n) {
        std::cerr << "Invalid edge: " << u << " --(" << w << ")--> " << v
                  << std::endl;
        return;
    }
    adj[u].push_back(std::make_pair(v, w));
    adj[v].push_back(std::make_pair(u, w));
}

void Graph::addSingleEdge(int u, int v, double w) {
    if (u < 0 || v < 0 || u >= n || v >= n) {
        std::cerr << "Invalid edge: " << u << " --(" << w << ")--> " << v
                  << std::endl;
        return;
    }
    adj[u].push_back(std::make_pair(v, w));
}

void Graph::allPairsShortest() {
    for (int i = 0; i < n; i++) {
        trueDist[i][i] = 0;
        for (auto& [v, w] : adj[i]) {
            trueDist[i][v] = w;
        }
    }

    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (trueDist[i][k] < INF && trueDist[k][j] < INF) {
                    trueDist[i][j] = std::min(trueDist[i][j],
                                              trueDist[i][k] + trueDist[k][j]);
                }
            }
        }
    }
}
