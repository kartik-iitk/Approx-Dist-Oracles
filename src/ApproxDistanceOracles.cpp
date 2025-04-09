#include "ApproxDistanceOracles.h"

ApproxDistanceOracles::ApproxDistanceOracles(Graph* graph, int k, bool tm)
    : graph(graph), k(k), test_mode(tm) {
    if (k <= 0) {
        std::cerr << "Invalid number of levels: " << k << std::endl;
        return;
    }

    landmarks.resize(k + 1);
    rank.resize(graph->n, 0);
    focus.resize(graph->n, std::vector<int>(k, -1));
    focus_distance.resize(
        graph->n,
        std::vector<double>(k, std::numeric_limits<double>::infinity()));
    ball.resize(graph->n, std::vector<std::queue<std::pair<int, double>>>(k));
    ball_map.resize(
        graph->n, fph::MetaFphMap<int, double, fph::meta::MixSeedHash<int>>());
}

void ApproxDistanceOracles::updateRank() {
    for (int i = 0; i < k; i++) {
        for (int v : landmarks[i]) {
            rank[v] = i;
        }
    }
}

void ApproxDistanceOracles::chooseLandmarks() {
    landmarks[0].resize(graph->n);
    iota(landmarks[0].begin(), landmarks[0].end(), 0);

    std::mt19937_64 generator(42);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    double prob = pow((double)graph->n, -1.0 / k);

    for (int i = 1; i < k; i++) {
        for (int v : landmarks[i - 1]) {
            double r = distribution(generator);
            if (r < prob) {
                landmarks[i].push_back(v);
            }
        }
    }

    updateRank();
}

void ApproxDistanceOracles::printLandmarks() {
    for (int i = 0; i < k; i++) {
        std::cout << "Level " << i << " landmarks (" << landmarks[i].size()
                  << "): ";
        for (int v : landmarks[i]) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }
}

// Time Complexity: O((m + n)logn). Here, we would usually look for graphs
// with m > n. So the time complexity is O(mlogn).
void ApproxDistanceOracles::multiSourceDijkstra(const std::vector<int>& sources,
                                                int level) {
    std::priority_queue<std::pair<double, int>,
                        std::vector<std::pair<double, int>>,
                        std::greater<std::pair<double, int>>>
        pq;
    std::vector<double> dist(graph->n, INF);
    std::vector<double> parent(
        graph->n, -1);  // Stores which of the sources led to the vertex.

    for (int src : sources) {
        if (src < 0 || src >= graph->n) {
            std::cerr << "Invalid source: " << src << std::endl;
            return;
        }
        pq.push({0, src});  // Equivalent to using dummy node.
        dist[src] = 0;
        parent[src] = src;  // The source's parent is itself.
    }

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;

        for (auto& [v, w] : graph->adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] =
                    parent[u];  // Set the original source that led to u.
                pq.push({dist[v], v});
            }
        }
    }

    // Update the focus arrays of all vertices.
    for (int v = 0; v < graph->n; v++) {
        focus[v][level - 1] = parent[v];
        focus_distance[v][level - 1] = dist[v];
    }
}

void ApproxDistanceOracles::printFocii() {
    for (int v = 0; v < graph->n; v++) {
        std::clog << "Focii of vertex " << v << ": ";
        for (int i = 0; i < k; i++) {
            std::clog << "(" << focus[v][i] << ", " << focus_distance[v][i]
                      << ") ";
        }
        std::clog << std::endl;
    }
}

void ApproxDistanceOracles::trimmedDijkstra(int v, int level) {
    std::priority_queue<std::pair<double, int>,
                        std::vector<std::pair<double, int>>,
                        std::greater<std::pair<double, int>>>
        pq;
    std::vector<double> dist(graph->n, INF);

    pq.push({0.0, v});
    dist[v] = 0.0;

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        for (auto& [w, wgt] : graph->adj[u]) {
            double nd = d + wgt;
            if (nd < dist[w] && nd < focus_distance[w][level]) {
                // 2nd condition is the group condition.
                dist[w] = nd;
                pq.push({nd, w});
            }
        }
    }

    // Update the ball of all vertices in the group.
    for (int w = 0; w < graph->n; w++) {
        if (dist[w] < INF) {
            ball[w][level].push({v, dist[w]});
        }
    }
}

void ApproxDistanceOracles::printBalls() {
    for (int v = 0; v < graph->n; v++) {
        std::clog << "Ball of vertex " << v << ": " << std::endl;
        for (int i = 0; i < k; i++) {
            int q_size = ball[v][i].size();
            std::clog << "  Level " << i << ": ";
            for (int iter = 0; iter < q_size; iter++) {
                auto it = ball[v][i].front();
                std::clog << "(" << it.first << ", " << it.second << ") ";
                ball[v][i].pop();
                ball[v][i].push(it);
            }
            std::clog << std::endl;
        }
        std::clog << std::endl;
    }
}

void ApproxDistanceOracles::hashBalls() {
    for (int v = 0; v < graph->n; v++) {
        std::vector<std::pair<int, double>> flat_data;
        for (auto& q : ball[v]) {
            int q_size = q.size();
            for (int iter = 0; iter < q_size; iter++) {
                auto it = q.front();
                flat_data.push_back(it);
                q.pop();
                q.push(it);
            }
        }
        ball_map[v].insert(flat_data.begin(), flat_data.end());
        ball_map[v].rehash(ball_map[v].size());
    }
}

void ApproxDistanceOracles::preprocess(
    const std::vector<std::vector<int>>& cust_land) {
    if (test_mode == 1) {
        for (int i = 0; i < k; i++) {
            landmarks[i] = cust_land[i];
        }
        updateRank();
    } else {
        chooseLandmarks();
    }

    for (int i = 1; i < k; i++) {
        multiSourceDijkstra(landmarks[i], i);
    }
    // For i = k, we don't run above loop as the focus is assumed at
    // infinity. This ensures that when we compute the groups while running
    // trimmedDijkstra, we will equivalently run plain Dijkstra for the last
    // level. Thus, the outermost ball of every vertex stores the distance
    // to all the highest rank landmark vertices.

    for (int i = 0; i < k; i++) {
        for (int v : landmarks[i]) {
            trimmedDijkstra(v, i);
        }
    }
    std::clog << "Trimmed Dijstra completed." << std::endl;
    hashBalls();
    std::clog << "Hash Balls completed." << std::endl;
}

double ApproxDistanceOracles::query(int u, int v) {
    if (u < 0 || v < 0 || u >= graph->n || v >= graph->n) {
        std::cerr << "Invalid query: " << u << " --?--> " << v << std::endl;
        return INF;
    }
    if (u == v) return 0.0;

    int i = 0;
    int w = u;
    while (!ball_map[v].contains(w)) {
        i++;
        if (i >= k) break;
        std::swap(u, v);
        w = focus[u][i - 1];
        if (w < 0) break;
    }

    double d_u_w;
    if (u == w) {
        d_u_w = 0.0;
    } else if (ball_map[u].contains(w)) {
        d_u_w = ball_map[u][w];
    } else {
        d_u_w = INF;
    }

    double d_v_w;
    if (v == w) {
        d_v_w = 0.0;
    } else if (ball_map[v].contains(w)) {
        d_v_w = ball_map[v][w];
    } else {
        d_v_w = INF;
    }

    if (d_u_w < INF && d_v_w < INF)
        return d_u_w + d_v_w;
    else
        return INF;
}