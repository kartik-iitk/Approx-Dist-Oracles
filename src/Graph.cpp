#include "Graph.h"

Graph::Graph(long long n) : n(n) {
    if (n <= 0) {
        std::cerr << "Invalid number of vertices: " << n << std::endl;
        return;
    }
    adj.resize(n);
    // trueDist.resize(n, std::vector<double>(n, INF));
}

void Graph::addBothEdges(long long u, long long v, double w) {
    if (u < 0 || v < 0 || u >= n || v >= n) {
        std::cerr << "Invalid edge: " << u << " --(" << w << ")--> " << v
                  << std::endl;
        return;
    }
    adj[u].push_back(std::make_pair(v, w));
    adj[v].push_back(std::make_pair(u, w));
}

void Graph::addSingleEdge(long long u, long long v, double w) {
    if (u < 0 || v < 0 || u >= n || v >= n) {
        std::cerr << "Invalid edge: " << u << " --(" << w << ")--> " << v
                  << std::endl;
        return;
    }
    adj[u].push_back(std::make_pair(v, w));
}

void Graph::allPairsShortest() {
    trueDist.resize(n, std::vector<double>(n, INF));

    for (long long i = 0; i < n; i++) {
        trueDist[i][i] = 0;
        for (auto& [v, w] : adj[i]) {
            trueDist[i][v] = w;
        }
    }

    for (long long k = 0; k < n; k++) {
        for (long long i = 0; i < n; i++) {
            for (long long j = 0; j < n; j++) {
                if (trueDist[i][k] < INF && trueDist[k][j] < INF) {
                    trueDist[i][j] = std::min(trueDist[i][j],
                                              trueDist[i][k] + trueDist[k][j]);
                }
            }
        }
    }
}

double Graph::dijkstra(long long u, long long v) {
    if (u < 0 || v < 0 || u >= n || v >= n) {
        std::cerr << "Invalid vertices: " << u << " and " << v << std::endl;
        return INF;
    }
    std::vector<double> dist(n, INF);
    dist[u] = 0;
    std::vector<bool> visited(n, false);
    for (long long i = 0; i < n; i++) {
        long long minIndex = -1;
        double minDist = INF;
        for (long long j = 0; j < n; j++) {
            if (!visited[j] && dist[j] < minDist) {
                minDist = dist[j];
                minIndex = j;
            }
        }
        if (minIndex == -1) break;
        visited[minIndex] = true;
        for (auto& [neighbor, weight] : adj[minIndex]) {
            if (!visited[neighbor]) {
                dist[neighbor] =
                    std::min(dist[neighbor], dist[minIndex] + weight);
            }
        }
    }
    return dist[v];
}