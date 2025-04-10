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
    in.close();
}