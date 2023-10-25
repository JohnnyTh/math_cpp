#include <chrono>
#include <iostream>
#include "spdlog/spdlog.h"

namespace timer {
    class Timer {

    private:
        std::vector<std::pair<std::string, long>> timingEntries;

    public:
        void timeit(const std::string &name, std::chrono::time_point<std::chrono::system_clock> start) {
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            timingEntries.emplace_back(name, elapsed);
        }

        void logTime() {

            for (auto &entry: timingEntries) {
                spdlog::info("'{}' executed in {} ms", entry.first, entry.second);
            }
            // Clear the entries after printing
            timingEntries.clear();
        }
    };
}