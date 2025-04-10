#include <main.h>

void processQueries(const std::string& testcase_path, const Parameters& params,
                    Graph& graph) {
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

    int u, v;
    while (queriesFile >> u >> v) {
        if (params.one_indexed) {
            u--;
            v--;
        }
        double exactDistance = graph.trueDist[u][v];
        outFile << exactDistance << std::endl;
    }

    queriesFile.close();
    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Too few input arguments. Need at least 2, received "
                  << argc << "." << std::endl;
        return 1;
    }

    std::string testcase_path = argv[1];

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

    graph.allPairsShortest();
    std::clog << "All pairs shortest paths computed." << std::endl;

    // Process queries
    processQueries(testcase_path, params, graph);
    std::clog << "True Distance processed." << std::endl;

    return 0;
}