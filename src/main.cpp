#include <bits/stdc++.h>

using namespace std;

const double INF = numeric_limits<double>::infinity();

class Graph {
   public:
    int n;
    vector<vector<pair<int, double>>> adj;  // Adjacency List

    Graph(int n) : n(n) {
        if (n <= 0) {
            cerr << "Invalid number of vertices: " << n << endl;
            return;
        }
        adj.resize(n);
    }

    void addEdge(int u, int v, double w) {
        if (u < 0 || v < 0 || u >= n || v >= n) {
            cerr << "Invalid edge: " << u << " --(" << w << ")--> " << v
                 << endl;
            return;
        }
        clog << "Adding edge: " << u << " --(" << w << ")--> " << v << endl;
        adj[u].push_back(make_pair(v, w));
        adj[v].push_back(make_pair(u, w));
    }
};

class ApproxDistanceOracles {
   public:
    Graph* graph;
    int k;

    vector<vector<int>> landmarks;  // landmarks[i] = landmarks at level i
    vector<int> rank;               // rank[v] = highest level a vertex is in.
    vector<vector<int>> focus;  // focus[v][i] = focus of vertex v at level i
    vector<vector<double>> focus_distance;    // corresponding focus distances
    vector<unordered_map<int, double>> ball;  // ball[v] = ball of vertex v

    ApproxDistanceOracles(Graph* graph, int k) : graph(graph), k(k) {
        if (k <= 0) {
            cerr << "Invalid number of levels: " << k << endl;
            return;
        }

        landmarks.resize(k + 1);
        rank.resize(graph->n, 0);
        focus.resize(graph->n, vector<int>(k, -1));
        focus_distance.resize(graph->n, vector<double>(k, INF));
        ball.resize(graph->n);
    }

    void chooseLandmarks() {
        landmarks[0].resize(graph->n);
        iota(landmarks[0].begin(), landmarks[0].end(), 0);

        default_random_engine generator(42);
        uniform_real_distribution<double> distribution(0.0, 1.0);
        double prob = pow((double)graph->n, -1.0 / k);

        for (int i = 1; i < k; i++) {
            for (int v : landmarks[i - 1]) {
                double r = distribution(generator);
                if (r < prob) {
                    landmarks[i].push_back(v);
                }
            }
        }

        for (int i = 0; i < k; i++) {
            for (int v : landmarks[i]) {
                rank[v] = i;
            }
        }
    }

    void printLandmarks() {
        for (int i = 0; i < k; i++) {
            cout << "Level " << i << " landmarks: ";
            for (int v : landmarks[i]) {
                cout << v << " ";
            }
            cout << endl;
        }
    }

    void multiSourceDijkstra(const vector<int>& sources, int level) {
        // Check if sources are valid
        for (int src : sources) {
            if (src < 0 || src >= graph->n) {
                cerr << "Invalid source: " << src << endl;
                return;
            }
        }

        priority_queue<pair<double, int>, vector<pair<double, int>>,
                       greater<pair<double, int>>>
            pq;
        vector<double> dist(graph->n, INF);

        for (int src : sources) {
            pq.push({0, src});
            dist[src] = 0;
            focus[src][level] = src;
            focus_distance[src][level] = 0.0;
        }

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;

            for (auto& [v, w] : graph->adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    pq.push({dist[v], v});
                    focus[v][level] = focus[u][level];
                    focus_distance[v][level] = dist[v];
                }
            }
        }
    }

    void printFocii() {
        for (int v = 0; v < graph->n; v++) {
            clog << "Focii of vertex " << v << ": ";
            for (int i = 0; i < k; i++) {
                clog << "(" << focus[v][i] << ", " << focus_distance[v][i]
                     << ") ";
            }
            clog << endl;
        }
    }

    void truncatedDijkstra(int v, int level) {
        // Define the threshold for truncation for vertex v.
        double threshold =
            (level == k - 1) ? INF : focus_distance[v][level + 1];

        // clog << "For vertex: " << v << " level: " << level << " " <<
        // threshold
        //      << endl;
        priority_queue<pair<double, int>, vector<pair<double, int>>,
                       greater<pair<double, int>>>
            pq;
        vector<double> dist(graph->n, INF);

        pq.push({0.0, v});
        dist[v] = 0.0;

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;
            // Truncate if the current distance reaches the threshold.
            if (d >= threshold) continue;

            for (auto& [w, wgt] : graph->adj[u]) {
                double nd = d + wgt;
                // Do not relax paths that exceed the threshold.
                if (nd >= threshold) continue;
                // For levels with a next-level focus, ensure the new distance
                // is less than the focus distance.
                if (level < k - 1 && nd >= focus_distance[w][level + 1])
                    continue;
                if (nd < dist[w]) {
                    dist[w] = nd;
                    pq.push({nd, w});
                }
            }
        }

        // Populate the ball for vertex v with only those vertices with a valid
        // distance.
        for (int u = 0; u < graph->n; u++) {
            if (u == v) continue;
            if (dist[u] < threshold && rank[u] == level &&
                (level == k - 1 || dist[u] < focus_distance[u][level + 1])) {
                ball[v][u] = dist[u];
            }
        }
    }

    void printBalls() {
        for (int v = 0; v < graph->n; v++) {
            clog << "Ball of vertex " << v << ": ";
            for (auto& [u, d] : ball[v]) {
                clog << "(" << u << ", " << d << ") ";
            }
            clog << endl;
        }
    }

    void preprocess() {
        chooseLandmarks();

        for (int i = 0; i < k; i++) {
            multiSourceDijkstra(landmarks[i], i);
        }

        for (int v = 0; v < graph->n; v++) {
            for (int i = 0; i < k; i++) {
                truncatedDijkstra(v, i);
            }
        }
    }

    double query(int u, int v) {
        if (u == v) return 0.0;

        int i = 0;
        int a = u, b = v;
        int w = u;
        while (ball[v].find(w) == ball[v].end()) {
            i++;
            if (i >= k) break;
            swap(u, v);
            w = focus[u][i];
            if (w < 0) break;
        }

        double d_u_w = (i < k ? focus_distance[u][i] : INF);
        double d_v_w = (ball[v].find(w) != ball[v].end() ? ball[v][w] : INF);

        if (d_u_w < INF && d_v_w < INF) return d_u_w + d_v_w;

        return focus_distance[a][k - 1] + focus_distance[b][k - 1];
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Invalid input arguments." << endl;
        return 1;
    }
    string parameter_file = argv[1];
    string graph_file = argv[2];
    string query_file = argv[3];

    ifstream paramFile(parameter_file);
    if (!paramFile.is_open()) {
        cerr << "Unable to open parameter file: " << parameter_file << endl;
        return 1;
    }

    int n, k;
    bool test_mode;
    paramFile >> n >> k >> test_mode;
    paramFile.close();

    clog << "Read parameters: n=" << n << ", k=" << k
         << ", test_mode=" << test_mode << endl;

    Graph graph(n);

    ifstream graphFile(graph_file);
    if (!graphFile.is_open()) {
        cerr << "Unable to open graph file: " << graph_file << endl;
        return 1;
    }

    int u, v, m = 0;
    double w;
    while (graphFile >> u >> v >> w) {
        graph.addEdge(u, v, w);
        m++;
    }
    graphFile.close();

    clog << "Read graph with " << m << " edges." << endl;

    ApproxDistanceOracles oracle(&graph, k);
    oracle.preprocess();
    // oracle.printLandmarks();
    // oracle.printFocii();
    // oracle.printBalls();
    cout << oracle.query(2, 4) << endl;

    return 0;
}