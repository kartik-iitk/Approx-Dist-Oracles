#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

#include "ApproxDistanceOracles.h"
#include "Benchmark.h"
#include "Graph.h"

struct TestCaseFiles {
    const std::string parameters = "/inp/parameters.txt";
    const std::string graph = "/inp/graph.txt";
    const std::string queries = "/inp/queries.txt";
    const std::string landmarks = "/inp/landmarks.txt";
    const std::string answers = "/out/answers.txt";
    const std::string distance = "/out/distance.txt";
    const std::string log = "/out/log.txt";
    const std::string runstats = "/out/runstats.txt";
};

struct Parameters {
    int n;
    int k;
    int num_threads;
    bool one_indexed;
    bool both;
    bool spaceopt;
    bool verbose;
    bool debug;
};

const TestCaseFiles fname;

bool validateTestCaseFolderStructure(const std::string& testcase_path) {
    namespace fs = std::filesystem;

    // Define the required folder and file structure
    std::vector<std::string> required_files = {fname.parameters, fname.graph,
                                               fname.queries, fname.landmarks};

    std::vector<std::string> required_folders = {"/inp"};

    // Check if the base folder exists
    if (!fs::exists(testcase_path) || !fs::is_directory(testcase_path)) {
        std::cerr << "Test case path does not exist or is not a directory: "
                  << testcase_path << std::endl;
        return false;
    }

    // Check for required folders
    for (const auto& folder : required_folders) {
        if (!fs::exists(testcase_path + folder) ||
            !fs::is_directory(testcase_path + folder)) {
            std::cerr << "Missing required folder: " << testcase_path + folder
                      << std::endl;
            return false;
        }
    }

    std::string outFolder = testcase_path + "/out";
    if (fs::exists(outFolder)) {
        fs::remove_all(outFolder);
    }
    fs::create_directory(outFolder);

    // Check for required files
    for (const auto& file : required_files) {
        if (!fs::exists(testcase_path + file) ||
            !fs::is_regular_file(testcase_path + file)) {
            if (file == fname.landmarks) {
                // Create an empty landmarks file if it doesn't exist
                std::ofstream emptyFile(testcase_path + file);
                if (!emptyFile.is_open()) {
                    std::cerr << "Failed to create missing landmarks file: "
                              << testcase_path + file << std::endl;
                    return false;
                }
                emptyFile.close();
                std::clog << "Created empty landmarks file: "
                          << testcase_path + file << std::endl;
            } else {
                std::cerr << "Missing required file: " << testcase_path + file
                          << std::endl;
                return false;
            }
        }
    }

    return true;
}

Parameters readParameters(const std::string& filepath) {
    Parameters params;
    std::ifstream in(filepath);
    if (!in.is_open()) {
        std::cerr << "Unable to open parameters file: " << filepath
                  << std::endl;
        exit(1);
    }

    if (!(in >> params.n >> params.k >> params.num_threads >>
          params.one_indexed >> params.both >> params.spaceopt >>
          params.verbose >> params.debug)) {
        std::cerr << "Error reading parameters from: " << filepath << std::endl;
        exit(1);
    }

    if (params.n <= 0 || params.k <= 0 || params.num_threads <= 0) {
        std::cerr << "Invalid parameters: n, k, and num_threads must be > 0."
                  << std::endl;
        exit(1);
    }

    return params;
}

void loadGraph(Graph& graph, const std::string& filepath, Parameters& params) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        std::cerr << "Unable to open graph file: " << filepath << std::endl;
        exit(1);
    }
    int u, v;
    double w;

    if (params.both) {
        while (in >> u >> v >> w) {
            if (params.one_indexed == 1) {
                u--;
                v--;
            }
            graph.addSingleEdge(u, v, w);
        }
    } else {
        while (in >> u >> v >> w) {
            if (params.one_indexed == 1) {
                u--;
                v--;
            }
            graph.addBothEdges(u, v, w);
        }
    }
}

void readLandmarks(const std::string& filepath, Parameters& params,
                   std::vector<std::vector<int>>& custom_landmarks) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        std::cerr << "Unable to open landmarks file: " << filepath << std::endl;
        exit(1);
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        int count;
        if (!(iss >> count)) {
            std::cerr << "Error reading landmarks count in line: " << line
                      << std::endl;
            exit(1);
        }
        std::vector<int> level;
        for (int i = 0; i < count; i++) {
            int landmark;
            if (!(iss >> landmark)) {
                std::cerr << "Error reading landmark index in line: " << line
                          << std::endl;
                exit(1);
            }
            if (params.one_indexed) landmark--;
            level.push_back(landmark);
        }
        custom_landmarks.push_back(level);
    }
    if (int(custom_landmarks.size()) != params.k) {
        std::cerr << "Invalid landmarks file: need all " << params.k
                  << " levels, but found " << custom_landmarks.size()
                  << std::endl;
        exit(1);
    }
}

void processQueries(const std::string& testcase_path, const Parameters& params,
                    ApproxDistanceOracles& oracle) {
    std::ifstream queriesFile(testcase_path + fname.queries);
    if (!queriesFile.is_open()) {
        std::cerr << "Unable to open queries file: "
                  << testcase_path + fname.queries << std::endl;
        exit(1);
    }

    std::ofstream outFile(testcase_path + fname.answers, std::ios::trunc);
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
        double approxDistance = oracle.query(u, v);
        outFile << approxDistance << std::endl;
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
    if (!validateTestCaseFolderStructure(testcase_path)) {
        std::cerr << "Invalid test case folder structure." << std::endl;
        return 1;
    }

    // Redirect both std::clog and std::cerr to log.txt file
    std::ofstream logFile(testcase_path + fname.log, std::ios::trunc);
    if (!logFile.is_open()) {
        std::cerr << "Unable to open log file: " << testcase_path + fname.log
                  << std::endl;
        return 1;
    }
    std::clog.rdbuf(logFile.rdbuf());
    std::cerr.rdbuf(logFile.rdbuf());

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

    // Create ApproxDistanceOracles
    ApproxDistanceOracles oracle(&graph, params.k, params.spaceopt,
                                 params.debug);

    std::vector<std::vector<int>> cust_landmarks;
    if (params.debug == 1) {
        std::string landmarksPath = testcase_path + fname.landmarks;
        readLandmarks(landmarksPath, params, cust_landmarks);
        std::clog << "Read custom landmarks." << std::endl;
    }

    // Create the oracle
    oracle.preprocess(cust_landmarks);
    std::clog << "Preprocessing completed." << std::endl;

    // Print focii, landmarks, and balls
    if (params.verbose) {
        std::clog << std::endl;
        oracle.printFocii();
        std::clog << std::endl;
        oracle.printLandmarks();
        std::clog << std::endl;
        oracle.printBalls();
        std::clog << std::endl;
    }

    // Process queries
    processQueries(testcase_path, params, oracle);
    std::clog << "Queries processed." << std::endl;

    return 0;
}