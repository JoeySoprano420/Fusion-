// fusionpp_runtime.cpp
#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>

namespace fusionpp {

// ============================
// Configuration Macros
// ============================
#ifndef FUSIONPP_ENABLE_PROFILING
#define FUSIONPP_ENABLE_PROFILING 1
#endif

#ifndef FUSIONPP_CSV_LOG_PATH
#define FUSIONPP_CSV_LOG_PATH "fusion_mode_transitions.csv"
#endif

enum class AIMode {
    GPT,
    LoRA
};

std::string modeToString(AIMode mode) {
    switch (mode) {
        case AIMode::GPT: return "GPT";
        case AIMode::LoRA: return "LoRA";
        default: return "UNKNOWN";
    }
}

#if FUSIONPP_ENABLE_PROFILING
class ScopedProfiler {
public:
    ScopedProfiler(const std::string& label)
        : label_(label), start_(std::chrono::high_resolution_clock::now()) {}
    ~ScopedProfiler() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cerr << "[Profiler] " << label_ << ": " << duration << " µs\n";
    }

private:
    std::string label_;
    std::chrono::high_resolution_clock::time_point start_;
};
#define FUSIONPP_PROFILE_SCOPE(name) ScopedProfiler profiler__##__LINE__(name)
#else
#define FUSIONPP_PROFILE_SCOPE(name)
#endif

class AIModeTracer {
public:
    AIModeTracer(AIMode from, AIMode to) : fromMode(from), toMode(to) {
        timestamp = std::chrono::system_clock::now();
    }

    ~AIModeTracer() {
        std::ofstream out(FUSIONPP_CSV_LOG_PATH, std::ios::app);
        auto timeT = std::chrono::system_clock::to_time_t(timestamp);
        out << std::put_time(std::localtime(&timeT), "%F %T") << ','
            << modeToString(fromMode) << ',' << modeToString(toMode) << '\n';
    }

private:
    AIMode fromMode;
    AIMode toMode;
    std::chrono::system_clock::time_point timestamp;
};

class RuntimeConfig {
public:
    static RuntimeConfig& getInstance() {
        static RuntimeConfig instance;
        return instance;
    }

    void setMode(const std::string& modeStr) {
        FUSIONPP_PROFILE_SCOPE("setMode");

        static const std::unordered_map<std::string, AIMode> modeMap = {
            {"gpt", AIMode::GPT},
            {"lora", AIMode::LoRA}
        };

        auto it = modeMap.find(toLower(modeStr));
        if (it == modeMap.end()) {
            throw std::invalid_argument("Unsupported AI mode: " + modeStr);
        }

        AIMode newMode = it->second;
        AIMode prevMode = currentMode.load();

        if (newMode != prevMode) {
            AIModeTracer tracer(prevMode, newMode); // RAII-style trace logger
            std::lock_guard<std::mutex> lock(modeMutex);
            currentMode.store(newMode);
        }
    }

    AIMode getMode() const {
        return currentMode.load();
    }

    bool isUsingLoRA() const {
        return currentMode.load() == AIMode::LoRA;
    }

private:
    RuntimeConfig() : currentMode(AIMode::GPT) {
        std::ofstream out(FUSIONPP_CSV_LOG_PATH, std::ios::out);
        out << "Timestamp,From,To\n"; // CSV header
    }

    std::string toLower(const std::string& str) const {
        std::string result = str;
        for (auto& c : result) c = static_cast<char>(tolower(c));
        return result;
    }

    std::atomic<AIMode> currentMode;
    mutable std::mutex modeMutex;
};

} // namespace fusionpp

#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

class AIModeTracer {
public:
    AIModeTracer(AIMode from, AIMode to, const std::string& segment)
        : fromMode(from), toMode(to), missionSegment(segment) {
        timestamp = std::chrono::system_clock::now();
        startMemory = getMemoryUsage();
        startCPU = getCPUUsage();
    }

    ~AIModeTracer() {
        long endMemory = getMemoryUsage();
        double endCPU = getCPUUsage();
        long deltaMemKB = endMemory - startMemory;
        double deltaCPU = endCPU - startCPU;

        std::ofstream out(FUSIONPP_CSV_LOG_PATH, std::ios::app);
        auto timeT = std::chrono::system_clock::to_time_t(timestamp);
        out << std::put_time(std::localtime(&timeT), "%F %T") << ','
            << missionSegment << ','
            << modeToString(fromMode) << ','
            << modeToString(toMode) << ','
            << deltaMemKB << ','
            << std::fixed << std::setprecision(2) << deltaCPU << '\n';
    }

private:
    AIMode fromMode, toMode;
    std::string missionSegment;
    std::chrono::system_clock::time_point timestamp;
    long startMemory;
    double startCPU;

    long getMemoryUsage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss; // in kilobytes
    }

    double getCPUUsage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_utime.tv_sec + usage.ru_stime.tv_sec +
               (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1e6;
    }
};

