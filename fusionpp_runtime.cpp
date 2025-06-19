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

// In constructor:
out << "Timestamp,Segment,From,To,MemoryDelta(KB),CPUDelta(s)\n";

// Usage:
AIModeTracer tracer(prevMode, newMode, currentSegment);

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <nlohmann/json.hpp>  // Download single_include/json.hpp from nlohmann

// ========== AI Runtime Modes ==========
enum class AIMode { GPT, LORA };
AIMode aiMode = AIMode::GPT;

void toggle_ai_mode(const std::string& mode) {
    if (mode == "gpt") aiMode = AIMode::GPT;
    else if (mode == "lora") aiMode = AIMode::LORA;
    std::cout << "[🌌] AI mode set to: " << (aiMode == AIMode::GPT ? "GPT" : "LoRA") << std::endl;
}

std::string ai_complete(const std::string& prompt) {
    if (aiMode == AIMode::GPT) {
        // Placeholder: Call to OpenAI API, subprocess, or HTTP client
        return "// [GPT-4] Completion for: " + prompt + "\nfunc generated_by_gpt\n  print \"Hello from GPT\"\nendfunc\n";
    } else {
        // Placeholder: Call to local LoRA inference
        return "// [LoRA] Completion for: " + prompt + "\nfunc generated_by_lora\n  print \"Hello from LoRA\"\nendfunc\n";
    }
}

// ========== Input & Action Binding ==========
std::unordered_map<std::string, std::function<void()>> inputActions;

void bind_input(const std::string& key, std::function<void()> handler) {
    inputActions[key] = handler;
    std::cout << "[🎮] Bound input: " << key << std::endl;
}

void input_loop() {
    std::string cmd;
    std::cout << "Type a command (type 'help' for options):\n";
    while (true) {
        std::getline(std::cin, cmd);
        if (cmd == "exit") break;
        if (cmd == "help") {
            std::cout << "Commands: reload, toggle_gpt, toggle_lora, run_script, show_log, exit\n";
            continue;
        }
        if (inputActions.count(cmd)) inputActions[cmd]();
        else std::cout << "[!] Unknown command.\n";
    }
}

// ========== Bytecode Runtime Simulation ==========
struct Bytecode {
    std::string op;
    std::vector<std::string> args;
};

using BytecodeFunc = std::vector<Bytecode>;

std::unordered_map<std::string, BytecodeFunc> bytecodeRegistry;

void register_bytecode(const std::string& name, const BytecodeFunc& fn) {
    bytecodeRegistry[name] = fn;
}

void run_bytecode(const std::string& name) {
    if (!bytecodeRegistry.count(name)) {
        std::cout << "[❌] Bytecode function not found: " << name << std::endl;
        return;
    }
    std::cout << "[🚦] Running bytecode for: " << name << std::endl;
    for (auto& b : bytecodeRegistry[name]) {
        if (b.op == "print") std::cout << "[PRINT] " << b.args[0] << std::endl;
        // Add more simulated ops here
    }
}

// ========== Hot-Reload & Log ==========
std::mutex reloadMutex;
std::string lastScript = "";

void reload_script() {
    std::lock_guard<std::mutex> lock(reloadMutex);
    std::ifstream in("autogen.fpp");
    if (!in) { std::cout << "[❌] No autogen.fpp script found.\n"; return; }
    std::string line, content;
    while (std::getline(in, line)) content += line + "\n";
    lastScript = content;
    std::cout << "[🔄] Reloaded script from autogen.fpp\n";
}

void show_log() {
    std::ifstream f("reinforce_log.json");
    if (!f) { std::cout << "[❌] No reinforce_log.json found.\n"; return; }
    std::string line;
    while (std::getline(f, line)) std::cout << line << "\n";
}

// ========== Cutscene/Cinematic Scripting ==========
void run_cutscene() {
    std::ifstream in("cinematic.fpp");
    if (!in) { std::cout << "[❌] cinematic.fpp not found.\n"; return; }
    std::string line;
    while (std::getline(in, line)) {
        if (line.find("camera move_to") != std::string::npos)
            std::cout << "[🎬] Camera moving...\n";
        else if (line.find("camera focus") != std::string::npos)
            std::cout << "[🎬] Camera focusing...\n";
        else if (line.find("camera shake") != std::string::npos)
            std::cout << "[🎬] Camera shaking...\n";
        else if (line.find("print") != std::string::npos)
            std::cout << "[CINEMATIC] " << line << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

// ========== Main ==========
int main() {
    bind_input("reload", reload_script);
    bind_input("toggle_gpt", []() { toggle_ai_mode("gpt"); });
    bind_input("toggle_lora", []() { toggle_ai_mode("lora"); });
    bind_input("run_script", []() {
        std::cout << "[RUN] Executing last loaded script:\n";
        std::cout << lastScript << std::endl;
    });
    bind_input("run_cutscene", run_cutscene);
    bind_input("show_log", show_log);

    // Example: register a bytecode function for demo
    register_bytecode("hello", { { "print", {"Hello from bytecode!"} } });

    std::cout << "Fusion++ Runtime started. (Type 'help')\n";
    input_loop();

    std::cout << "[💾] Exiting. Bye!\n";
    return 0;
}
