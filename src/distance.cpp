#include <main.h>

void processQueries(const std::string& testcase_path, const Parameters& params,
                    Graph& graph, bool use_dijkstra) {
    std::ifstream queriesFile(testcase_path + fname.queries);
    if (!queriesFile.is_open()) {
        std::cerr << "Unable to open queries file: "
                  << testcase_path + fname.queries << std::endl;
        exit(1);
    }

    std::ofstream outFile(testcase_path + fname.distance, std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Unable to open distance file: "
                  << testcase_path + fname.distance << std::endl;
        exit(1);
    }

    long long u, v;
    while (queriesFile >> u >> v) {
        if (params.one_indexed) {
            u--;
            v--;
        }
        double exactDistance;
        if (!use_dijkstra) {
            exactDistance = graph.trueDist[u][v];
        } else {
            exactDistance = graph.dijkstra(u, v);
        }
        outFile << exactDistance << std::endl;
    }

    queriesFile.close();
    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Too few input arguments. Need at least 3, received "
                  << argc << "." << std::endl;
        return 1;
    }

    std::string testcase_path = argv[1];
    bool use_dijkstra = std::stoi(argv[2]);

    // Read parameters
    std::string parametersPath = testcase_path + fname.parameters;
    Parameters params = readParameters(parametersPath);
    std::clog << "Read parameters: n = " << params.n << ", k = " << params.k
              << ", num_threads = " << params.num_threads
              << ", one_indexed = " << params.one_indexed
              << ", both = " << params.both
              << ", spaceopt = " << params.spaceopt
              << ", verbose = " << params.verbose
              << ", debug = " << params.debug << std::endl;

    // Load graph
    Graph graph(params.n);
    std::string graphPath = testcase_path + fname.graph;
    loadGraph(graph, graphPath, params);
    std::clog << "Graph loaded with " << graph.n << " vertices." << std::endl;

    if (!use_dijkstra) {
        graph.allPairsShortest();
        std::clog << "All pairs shortest paths computed." << std::endl;
    }

    // Process queries
    processQueries(testcase_path, params, graph, use_dijkstra);
    std::clog << "True Distance processed." << std::endl;

    return 0;
}