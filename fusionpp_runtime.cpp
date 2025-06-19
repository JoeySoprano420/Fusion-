// fusionpp_runtime.cpp
#include <atomic>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <mutex>
#include <iostream>
#include <chrono>

namespace fusionpp {

// ============================
// Configuration Macros
// ============================
#ifndef FUSIONPP_ENABLE_PROFILING
#define FUSIONPP_ENABLE_PROFILING 1
#endif

enum class AIMode {
    GPT,
    LoRA
};

#if FUSIONPP_ENABLE_PROFILING
class ScopedProfiler {
public:
    ScopedProfiler(const std::string& label) : label_(label), start_(std::chrono::high_resolution_clock::now()) {}
    ~ScopedProfiler() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cerr << "[Profiler] " << label_ << ": " << duration << " µs\n";
    }
private:
    std::string label_;
    std::chrono::high_resolution_clock::time_point start_;
};
#define FUSIONPP_PROFILE_SCOPE(name) ScopedProfiler profiler##__LINE__(name)
#else
#define FUSIONPP_PROFILE_SCOPE(name)
#endif

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

        std::lock_guard<std::mutex> lock(modeMutex);
        currentMode.store(it->second);
    }

    AIMode getMode() const {
        return currentMode.load();
    }

    bool isUsingLoRA() const {
        return currentMode.load() == AIMode::LoRA;
    }

private:
    RuntimeConfig() : currentMode(AIMode::GPT) {}

    std::string toLower(const std::string& str) const {
        std::string result = str;
        for (auto& c : result) c = static_cast<char>(tolower(c));
        return result;
    }

    std::atomic<AIMode> currentMode;
    mutable std::mutex modeMutex;
};

} // namespace fusionpp
