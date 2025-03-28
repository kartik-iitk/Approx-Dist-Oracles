#include <fstream>

#include "ApproxDistanceOracles.h"
#include "Graph.h"

const int test_case =
    3;  // For different types of input, to allow for customisation. Using
        // default 1 follows the conventions of the README.

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Too few input arguments. Need at least 4, received "
                  << argc << "." << std::endl;
        return 1;
    }
    std::string parameter_file = argv[1];
    std::string graph_file = argv[2];
    std::string query_file = argv[3];

    std::ifstream paramFile(parameter_file);
    if (!paramFile.is_open()) {
        std::cerr << "Unable to open parameter file: " << parameter_file
                  << std::endl;
        return 1;
    }

    int n, k;
    bool test_mode;
    paramFile >> n >> k >> test_mode;
    paramFile.close();

    std::clog << "Read parameters: n=" << n << ", k=" << k
              << ", test_mode=" << test_mode << std::endl;

    Graph graph(n);

    std::ifstream graphFile(graph_file);
    if (!graphFile.is_open()) {
        std::cerr << "Unable to open graph file: " << graph_file << std::endl;
        return 1;
    }

    int u, v, m = 0;
    double w;
    while (graphFile >> u >> v >> w) {
        switch (test_case) {
            case 1:
                graph.addBothEdges(u, v, w);
                break;

            case 2:
                graph.addBothEdges(u, v, w);
                break;

            case 3:
                graph.addSingleEdge(u - 1, v - 1, w);
                break;
        }
        m++;
    }
    graphFile.close();

    std::clog << "Read graph with " << m << " edges." << std::endl;

    ApproxDistanceOracles oracle(&graph, k, test_mode);

    if (test_mode == 1) {
        if (argc != 6) {
            std::cerr
                << "In test mode, invalid input arguments. Needed 6, received "
                << argc << "." << std::endl;
            return 1;
        }
    }

    if (test_mode == 1) {
        std::string landmarks_file = argv[4];

        std::ifstream landFile(landmarks_file);
        if (!landFile.is_open()) {
            std::cerr << "Unable to open landmarks file: " << landmarks_file
                      << std::endl;
            return 1;
        }

        if (landFile.peek() == std::ifstream::traits_type::eof()) {
            std::cerr << "Landmarks file is empty." << std::endl;
            return 1;
        }

        std::vector<std::vector<int>> cust_land(k);
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
        oracle.preprocess(std::vector<std::vector<int>>());
    }

    oracle.printLandmarks();
    oracle.printFocii();
    oracle.printBalls();

    std::ifstream queryFile(query_file);
    if (!queryFile.is_open()) {
        std::cerr << "Unable to open queries file: " << query_file << std::endl;
        return 1;
    }

    std::string answers_file = argv[5];
    // First, check if the answers file is empty.
    std::ifstream answersFile_check(answers_file);
    bool generating_answers = false;
    if (!answersFile_check.good() ||
        answersFile_check.peek() == std::ifstream::traits_type::eof()) {
        generating_answers = true;
    }
    answersFile_check.close();

    if (generating_answers) {
        // If the file is empty, open an output stream to generate answers.
        std::ofstream answersFile_out(answers_file, std::ios::trunc);
        if (!answersFile_out.is_open()) {
            std::cerr << "Unable to open answers file: " << answers_file
                      << std::endl;
            return 1;
        }
        graph.allPairsShortest();
        while (queryFile >> u >> v) {
            switch (test_case) {
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    u--;
                    v--;
                    break;
            }
            double ans = oracle.query(u, v);
            double true_ans = graph.trueDist[u][v];
            answersFile_out << true_ans << std::endl;
            std::clog << "Query: " << u << " --> " << v << "; Result: " << ans
                      << "; True Distance: " << true_ans << std::endl;
        }
        answersFile_out.close();
    } else {
        // Otherwise, open an input stream to read the provided answers.
        std::ifstream answersFile_in(answers_file);
        if (!answersFile_in.is_open()) {
            std::cerr << "Unable to open answers file: " << answers_file
                      << std::endl;
            return 1;
        }
        while (queryFile >> u >> v) {
            switch (test_case) {
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    u--;
                    v--;
                    break;
            }
            double ans = oracle.query(u, v);
            double true_ans;
            if (answersFile_in >> true_ans) {
                if (ans < true_ans || ans > (2 * k - 1) * true_ans) {
                    std::cerr << "Incorrect answer for query: " << u
                              << " --?--> " << v << ". Expected: " << true_ans
                              << ", Received: " << ans << std::endl;
                } else {
                    std::clog << u << " " << v << ": " << ans << std::endl;
                }
            } else {
                std::cerr << "Too few answers in file. Rerun by emptying file: "
                          << answers_file << std::endl;
                return 1;
            }
        }
        answersFile_in.close();
    }

    queryFile.close();

    return 0;
}