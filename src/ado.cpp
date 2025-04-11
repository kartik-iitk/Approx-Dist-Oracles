#include <main.h>

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
        long long count;
        if (!(iss >> count)) {
            std::cerr << "Error reading landmarks count in line: " << line
                      << std::endl;
            exit(1);
        }
        std::vector<int> level;
        for (long long i = 0; i < count; i++) {
            long long landmark;
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
                    ApproxDistanceOracles& oracle, Benchmark& bench,
                    std::ofstream& statsFile) {
    std::ifstream queriesFile(testcase_path + fname.queries);
    if (!queriesFile.is_open()) {
        std::cerr << "Unable to open queries file: "
                  << testcase_path + fname.queries << std::endl;
        exit(1);
    }

    std::ofstream outFile(testcase_path + fname.answers, std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "Unable to open answers file: "
                  << testcase_path + fname.answers << std::endl;
        exit(1);
    }

    long long u, v;
    while (queriesFile >> u >> v) {
        if (params.one_indexed) {
            u--;
            v--;
        }
        bench.start();
        double approxDistance = oracle.query(u, v);
        bench.stop();
        statsFile << bench.elapsedNanoseconds() << std::endl;
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

    Benchmark bench;

    bench.start();
    // Create the oracle
    auto run_output = oracle.preprocess(cust_landmarks);
    bench.stop();

    std::ofstream statsFile(testcase_path + fname.runstats, std::ios::trunc);
    if (!statsFile.is_open()) {
        std::clog << "Unable to open stats file: "
                  << testcase_path + fname.runstats << std::endl;
        exit(1);
    }
    statsFile << run_output.first << std::endl;
    statsFile << bench.elapsedMilliseconds() << std::endl;
    statsFile << run_output.second << std::endl;

    std::clog << "Preprocessing completed in " << run_output.first
              << " run(s) in " << bench.elapsedMilliseconds() << "ms"
              << std::endl;

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
    processQueries(testcase_path, params, oracle, bench, statsFile);
    statsFile.close();
    std::clog << "Queries processed." << std::endl;

    return 0;
}