#include "ApproxDistanceOracles.h"

ApproxDistanceOracles::ApproxDistanceOracles(Graph* graph, long long k, bool sp,
                                             bool dbg)
    : graph(graph), k(k), spaceopt(sp), debug(dbg) {
    if (k <= 0) {
        std::cerr << "Invalid number of levels: " << k << std::endl;
        return;
    }

    landmarks.resize(k + 1);
    rank.resize(graph->n, 0);
    focus.resize(graph->n, std::vector<long long>(k, -1));
    focus_distance.resize(
        graph->n,
        std::vector<double>(k, std::numeric_limits<double>::infinity()));
    ball.resize(graph->n,
                std::vector<std::queue<std::pair<long long, double>>>(k));
    ball_map.resize(graph->n,
                    fph::MetaFphMap<long long, double,
                                    fph::meta::MixSeedHash<long long>>());
}

void ApproxDistanceOracles::updateRank() {
    for (long long i = 0; i < k; i++) {
        for (long long v : landmarks[i]) {
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

    for (long long i = 1; i < k; i++) {
        for (long long v : landmarks[i - 1]) {
            double r = distribution(generator);
            if (r < prob) {
                landmarks[i].push_back(v);
            }
        }
    }

    updateRank();
}

void ApproxDistanceOracles::printLandmarks() {
    for (long long i = 0; i < k; i++) {
        std::clog << "Level " << i << " landmarks (" << landmarks[i].size()
                  << "): ";
        for (long long v : landmarks[i]) {
            std::clog << v << " ";
        }
        std::clog << std::endl;
    }
}

// Time Complexity: O((m + n)logn). Here, we would usually look for graphs
// with m > n. So the time complexity is O(mlogn).
void ApproxDistanceOracles::multiSourceDijkstra(
    const std::vector<long long>& sources, long long level) {
    std::priority_queue<std::pair<double, long long>,
                        std::vector<std::pair<double, long long>>,
                        std::greater<std::pair<double, long long>>>
        pq;
    std::vector<double> dist(graph->n, INF);
    std::vector<long long> parent(
        graph->n, -1);  // Stores which of the sources led to the vertex.

    for (long long src : sources) {
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
    for (long long v = 0; v < graph->n; v++) {
        focus[v][level - 1] = parent[v];
        focus_distance[v][level - 1] = dist[v];
    }
}

void ApproxDistanceOracles::printFocii() {
    for (long long v = 0; v < graph->n; v++) {
        std::clog << "Focii of vertex " << v << ": ";
        for (long long i = 0; i < k; i++) {
            std::clog << "(" << focus[v][i] << ", " << focus_distance[v][i]
                      << ") ";
        }
        std::clog << std::endl;
    }
}

void ApproxDistanceOracles::trimmedDijkstra(long long v, long long level) {
    std::priority_queue<std::pair<double, long long>,
                        std::vector<std::pair<double, long long>>,
                        std::greater<std::pair<double, long long>>>
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
    for (long long w = 0; w < graph->n; w++) {
        if (dist[w] < INF) {
            ball[w][level].push({v, dist[w]});
        }
    }
}

void ApproxDistanceOracles::printBalls() {
    for (long long v = 0; v < graph->n; v++) {
        std::clog << "Ball of vertex " << v << ": " << std::endl;
        for (long long i = 0; i < k; i++) {
            long long q_size = ball[v][i].size();
            std::clog << "  Level " << i << ": ";
            for (long long iter = 0; iter < q_size; iter++) {
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

long long ApproxDistanceOracles::hashBalls() {
    long long total_size = 0;
    for (long long v = 0; v < graph->n; v++) {
        ball_map[v].clear();
        std::vector<std::pair<long long, double>> flat_data;
        for (auto& q : ball[v]) {
            long long q_size = q.size();
            for (long long iter = 0; iter < q_size; iter++) {
                auto it = q.front();
                flat_data.push_back(it);
                q.pop();
                q.push(it);
            }
        }
        ball_map[v].insert(flat_data.begin(), flat_data.end());
        ball_map[v].rehash(
            ball_map[v]
                .size());  // Above insertion removed duplicates, now make sure
                           // hashing is as effiicient as possible.
        total_size += ball_map[v].size();
    }
    return total_size;
}

std::pair<long long, long long> ApproxDistanceOracles::preprocess(
    const std::vector<std::vector<long long>>& cust_land) {
    long long total_space = 0, num_runs = 0;
    long long n_root =
        static_cast<long long>(std::ceil(pow(graph->n, 1.0 / k)));
    do {
        num_runs++;
        landmarks.assign(k + 1, std::vector<long long>());
        rank.assign(graph->n, 0);
        focus.assign(graph->n, std::vector<long long>(k, -1));
        focus_distance.assign(
            graph->n,
            std::vector<double>(k, std::numeric_limits<double>::infinity()));
        ball.assign(graph->n,
                    std::vector<std::queue<std::pair<long long, double>>>(k));
        ball_map.assign(graph->n,
                        fph::MetaFphMap<long long, double,
                                        fph::meta::MixSeedHash<long long>>());

        if (debug == 1 && spaceopt == 0) {
            for (long long i = 0; i < k; i++) {
                landmarks[i] = cust_land[i];
            }
            updateRank();
        } else {
            chooseLandmarks();
        }

        for (long long i = 1; i < k; i++) {
            multiSourceDijkstra(landmarks[i], i);
        }
        // For i = k, we don't run above loop as the focus is assumed at
        // infinity. This ensures that when we compute the groups while running
        // trimmedDijkstra, we will equivalently run plain Dijkstra for the last
        // level. Thus, the outermost ball of every vertex stores the distance
        // to all the highest rank landmark vertices.

        for (long long i = 0; i < k; i++) {
            for (long long v : landmarks[i]) {
                if (rank[v] >= i) {
                    // Run only when v can belong to that level ball.
                    trimmedDijkstra(v, i);
                }
            }
        }

        // Note that the above loop may not add the focii to the balls
        // of the vertices in a special case when the focus of the higher level
        // is at the same distance but is randomly chosen differently. Query
        // Processing Algorithm will mess up if this is not handled, (wrongly)
        // declaring that the distance is infinity. Now add each vertex’s focus
        // at level i into its ball at level i+1 (excluding the last level).
        for (long long v = 0; v < graph->n; ++v) {
            for (long long i = 0; i + 1 < k; ++i) {
                long long f = focus[v][i];
                double d = focus_distance[v][i];
                if (f >= 0 && d < INF) {
                    ball[v][i + 1].push({f, d});
                }
            }
        }

        total_space = hashBalls();
        std::clog << "Total space used: " << 2 * total_space + graph->n
                  << " words, and required limit: "
                  << 100LL * k * graph->n * n_root << " words" << std::endl;
    } while (spaceopt == 1 && total_space > 1000LL * k * graph->n * n_root);
    // 1000 was chosen arbitrarily.
    return std::make_pair(num_runs, 2 * total_space + graph->n);
}

double ApproxDistanceOracles::query(long long u, long long v) {
    if (u < 0 || v < 0 || u >= graph->n || v >= graph->n) {
        std::cerr << "Invalid query: " << u << " --?--> " << v << std::endl;
        return INF;
    }
    if (u == v) return 0.0;

    long long i = 0;
    long long w = u;
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