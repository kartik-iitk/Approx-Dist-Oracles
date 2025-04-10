#include "Benchmark.h"

// Starts the timer
void Benchmark::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

// Stops the timer
void Benchmark::stop() { endTime = std::chrono::high_resolution_clock::now(); }

double Benchmark::elapsedNanoseconds() const {
    return std::chrono::duration<double, std::nano>(endTime - startTime).count();
}

// Returns the elapsed time in milliseconds
double Benchmark::elapsedMilliseconds() const {
    return std::chrono::duration<double, std::milli>(endTime - startTime)
        .count();
}

// Returns the elapsed time in seconds
double Benchmark::elapsedSeconds() const {
    return std::chrono::duration<double>(endTime - startTime).count();
}