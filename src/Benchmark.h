#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <functional>
#include <iostream>

class Benchmark {
   public:
    void start();
    void stop();
    double elapsedMilliseconds() const;
    double elapsedSeconds() const;

   private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
};

#endif  // BENCHMARK_H