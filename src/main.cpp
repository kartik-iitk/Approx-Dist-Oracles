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
    bool test_mode;

    vector<vector<int>> landmarks;  // landmarks[i] = landmarks at level i
    vector<int> rank;               // rank[v] = highest level a vertex is in.
    vector<vector<int>> focus;  // focus[v][i] = focus of vertex v at level i
    vector<vector<double>> focus_distance;    // corresponding focus distances
    vector<unordered_map<int, double>> ball;  // ball[v] = ball of vertex v

    ApproxDistanceOracles(Graph* graph, int k, bool tm)
        : graph(graph), k(k), test_mode(tm) {
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

    void updateRank() {
        for (int i = 0; i < k; i++) {
            for (int v : landmarks[i]) {
                rank[v] = i;
            }
        }
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

        updateRank();
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

    // Time Complexity: O((m + n)logn). Here, we would usually look for graphs
    // with m > n. So the time complexity is O(mlogn).
    void multiSourceDijkstra(const vector<int>& sources, int level) {
        priority_queue<pair<double, int>, vector<pair<double, int>>,
                       greater<pair<double, int>>>
            pq;
        vector<double> dist(graph->n, INF);
        vector<double> parent(
            graph->n, -1);  // Stores which of the sources led to the vertex.

        for (int src : sources) {
            if (src < 0 || src >= graph->n) {
                cerr << "Invalid source: " << src << endl;
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

    void trimmedDijkstra(int v, int level) {
        priority_queue<pair<double, int>, vector<pair<double, int>>,
                       greater<pair<double, int>>>
            pq;
        vector<double> dist(graph->n, INF);

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
                    ball[w][v] = nd;  // Inserts if not present, else updates.
                }
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

    void preprocess(const vector<vector<int>>& cust_land) {
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
    }

    double query(int u, int v) {
        if (u < 0 || v < 0 || u >= graph->n || v >= graph->n) {
            cerr << "Invalid query: " << u << " --?--> " << v << endl;
            return INF;
        }
        if (u == v) return 0.0;

        int i = 0;
        int w = u;
        while (ball[v].find(w) == ball[v].end()) {
            i++;
            if (i >= k) break;
            swap(u, v);
            w = focus[u][i - 1];
            if (w < 0) break;
        }

        double d_u_w;
        if (u == w) {
            d_u_w = 0.0;
        } else if (ball[u].find(w) != ball[u].end()) {
            d_u_w = ball[u][w];
        } else {
            d_u_w = INF;
        }

        double d_v_w;
        if (v == w) {
            d_v_w = 0.0;
        } else if (ball[v].find(w) != ball[v].end()) {
            d_v_w = ball[v][w];
        } else {
            d_v_w = INF;
        }

        if (d_u_w < INF && d_v_w < INF)
            return d_u_w + d_v_w;
        else
            return INF;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Too few input arguments. Need at least 4, received " << argc
             << "." << endl;
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

    ApproxDistanceOracles oracle(&graph, k, test_mode);

    if (test_mode == 1) {
        if (argc != 6) {
            cerr << "In test mode, invalid input arguments. Needed 6, received "
                 << argc << "." << endl;
            return 1;
        }
    }

    if (test_mode == 1) {
        string landmarks_file = argv[4];

        ifstream landFile(landmarks_file);
        if (!landFile.is_open()) {
            cerr << "Unable to open landmarks file: " << landmarks_file << endl;
            return 1;
        }

        vector<vector<int>> cust_land(k);
        for (int i = 0; i < k; i++) {
            int l;
            landFile >> l;
            cust_land[i].resize(l);
            for (int j = 0; j < l; j++) {
                landFile >> cust_land[i][j];
            }
        }

        oracle.preprocess(cust_land);
    } else {
        oracle.preprocess(vector<vector<int>>());
    }

    oracle.printLandmarks();
    oracle.printFocii();
    oracle.printBalls();

    ifstream queryFile(query_file);
    if (!queryFile.is_open()) {
        cerr << "Unable to open queries file: " << query_file << endl;
        return 1;
    }

    string answers_file = argv[5];
    ifstream answersFile(answers_file);
    if (!answersFile.is_open()) {
        cerr << "Unable to open answers file: " << answers_file << endl;
        return 1;
    }

    u = 0;
    v = 0;
    while (queryFile >> u >> v) {
        double ans = oracle.query(u, v);
        // if (answersFile >> w) {
        //     if (abs(ans - w) > 1e-6) {
        //         cerr << "Incorrect answer for query: " << u << " --?--> " <<
        //         v
        //              << ". Expected: " << w << ", Received: " << ans << endl;
        //     }
        // } else {
        //     cerr << "Too few answers in file: " << answers_file << endl;
        //     return 1;
        // }
        clog << u << " " << v << ": " << ans << endl;
    }

    return 0;
}