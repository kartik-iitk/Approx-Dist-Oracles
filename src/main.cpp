#include <bits/stdc++.h>

using namespace std;

const double INF = numeric_limits<double>::infinity();

//
// Graph class: undirected weighted graph using an adjacency list.
//
class Graph {
   public:
    int n;  // number of vertices
    // Each element: (neighbor, weight)
    vector<vector<pair<int, double>>> adj;

    Graph(int n) : n(n) { adj.resize(n); }

    void addEdge(int u, int v, double w) {
        // Assumes 0-indexed vertices.
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }
};

//
// DistanceOracle class: builds an approximate distance oracle
// using a Thorup-Zwick style hierarchy. The parameter k controls the trade-off
// between preprocessing space/time and the stretch (approximation factor, ≤
// 2k-1).
//
class DistanceOracle {
   public:
    Graph &graph;
    int k;  // parameter (levels)
    int n;  // number of vertices

    // A[i] holds the set of vertices at level i.
    vector<vector<int>> A;
    // For level i (i = 0,1,..., k-1), for each vertex v,
    // store p[i][v]: the nearest vertex in A[i+1] (i.e. the pivot)
    // and d[i][v]: the distance from v to that pivot.
    vector<vector<int>> pivots;        // pivots[i][v]
    vector<vector<double>> pivotDist;  // pivotDist[i][v]

    // For every pivot vertex (from any level), store distances to all vertices.
    // This lets us answer queries quickly via a “hop” from u to a pivot.
    unordered_map<int, vector<double>> pivotDistances;

    // Standard single–source Dijkstra.
    vector<double> dijkstra(int source) {
        vector<double> dist(n, INF);
        // (distance, vertex)
        priority_queue<pair<double, int>, vector<pair<double, int>>,
                       greater<pair<double, int>>>
            pq;
        dist[source] = 0.0;
        pq.push({0.0, source});
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) continue;
            for (auto &edge : graph.adj[u]) {
                int v = edge.first;
                double nd = d + edge.second;
                if (nd < dist[v]) {
                    dist[v] = nd;
                    pq.push({nd, v});
                }
            }
        }
        return dist;
    }

   public:
    //
    // Constructor: builds the oracle from the input graph and parameter k.
    //
    DistanceOracle(Graph &g, int k) : graph(g), k(k), n(g.n) {
        // There will be k+1 levels: A[0] ... A[k]
        A.resize(k + 1);
        // Level 0: all vertices.
        for (int i = 0; i < n; i++) {
            A[0].push_back(i);
        }

        // Sample the remaining levels: for i>=1, each vertex in A[i-1] is
        // included in A[i] with probability n^(-1/k).
        double prob = pow(n, -1.0 / k);
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dis(0.0, 1.0);
        for (int i = 1; i <= k; i++) {
            for (int v : A[i - 1]) {
                if (dis(gen) < prob) {
                    A[i].push_back(v);
                }
            }
        }
        // Ensure the highest level A[k] is not empty.
        if (A[k].empty() && !A[k - 1].empty()) {
            A[k].push_back(A[k - 1][0]);
        }

        // Resize storage for pivots and distances.
        pivots.resize(k);
        pivotDist.resize(k, vector<double>(n, INF));

        // For each level i (0 to k-1), compute for every vertex v
        // its closest vertex in A[i+1] using multi–source Dijkstra.
        for (int i = 0; i < k; i++) {
            vector<double> dist(n, INF);
            vector<int> nearest(n, -1);
            // Priority queue: (distance, vertex)
            priority_queue<pair<double, int>, vector<pair<double, int>>,
                           greater<pair<double, int>>>
                pq;
            // All vertices in A[i+1] are sources with distance 0.
            for (int v : A[i + 1]) {
                dist[v] = 0.0;
                nearest[v] = v;
                pq.push({0.0, v});
            }
            // Run Dijkstra.
            while (!pq.empty()) {
                auto [d, u] = pq.top();
                pq.pop();
                if (d > dist[u]) continue;
                for (auto &edge : graph.adj[u]) {
                    int w = edge.first;
                    double nd = d + edge.second;
                    if (nd < dist[w]) {
                        dist[w] = nd;
                        nearest[w] = nearest[u];
                        pq.push({nd, w});
                    }
                }
            }
            // Store the nearest pivot and its distance for each vertex.
            for (int v = 0; v < n; v++) {
                pivots[i].push_back(nearest[v]);
                pivotDist[i][v] = dist[v];
            }
        }

        // Precompute distances from each pivot vertex (found in any level)
        // to all vertices in the graph. We avoid duplicate Dijkstra calls.
        unordered_map<int, bool> done;
        for (int i = 0; i < k; i++) {
            for (int v = 0; v < n; v++) {
                int p = pivots[i][v];
                if (p == -1) continue;  // Skip if no pivot was found.
                if (done.find(p) == done.end()) {
                    pivotDistances[p] = dijkstra(p);
                    done[p] = true;
                }
            }
        }
    }

    //
    // Query: returns an approximate distance between vertices u and v.
    // The query is answered by “hopping” from u (or v) to a pivot, and then
    // using precomputed distances from that pivot.
    //
    double query(int u, int v) {
        if (u == v) return 0.0;
        double best = INF;
        // Try pivots computed for u.
        for (int i = 0; i < k; i++) {
            int p = pivots[i][u];
            if (p == -1 || pivotDistances.find(p) == pivotDistances.end())
                continue;
            double d1 = pivotDist[i][u];       // distance from u to pivot p
            double d2 = pivotDistances[p][v];  // distance from pivot p to v
            best = min(best, d1 + d2);
        }
        // Also try pivots computed for v.
        for (int i = 0; i < k; i++) {
            int p = pivots[i][v];
            if (p == -1 || pivotDistances.find(p) == pivotDistances.end())
                continue;
            double d1 = pivotDist[i][v];       // distance from v to pivot p
            double d2 = pivotDistances[p][u];  // distance from pivot p to u
            best = min(best, d1 + d2);
        }
        return best;
    }
};

//
// Main: reads a graph, builds the oracle, and processes a single query.
// Expected input format:
//   n m k
//   (then m lines with: u v weight)
//   (then one query: u v)
//
int main() {
    int n, m, k;
    cout << "Enter number of vertices, edges, and parameter k: ";
    cin >> n >> m >> k;
    Graph g(n);
    cout << "Enter " << m
         << " edges (u v weight), vertices are 0-indexed:" << endl;
    for (int i = 0; i < m; i++) {
        int u, v;
        double w;
        cin >> u >> v >> w;
        g.addEdge(u, v, w);
    }
    // Build the distance oracle.
    DistanceOracle oracle(g, k);
    for (auto a : oracle.A[1]) {
        cout << a << " ";
    }
    cout << endl;
    while (true) {
        int u, v;
        cout << "Enter query vertices (u v): ";
        cin >> u >> v;
        double approxDist = oracle.query(u, v);
        if (approxDist == INF)
            cout << "No path exists between " << u << " and " << v << endl;
        else
            cout << "Approximate distance between " << u << " and " << v
                 << " is " << approxDist << endl;
    }
    return 0;
}