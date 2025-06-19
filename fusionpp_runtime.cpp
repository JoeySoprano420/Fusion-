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

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include <asio.hpp> // Use standalone Asio (https://think-async.com/Asio/)
using json = nlohmann::json;

// ====== AI Runtime Modes ======
enum class AIMode { GPT, LORA };
AIMode aiMode = AIMode::GPT;

void toggle_ai_mode(const std::string& mode) {
    aiMode = (mode == "lora") ? AIMode::LORA : AIMode::GPT;
    std::cout << "[🌌] AI mode set to: " << (aiMode == AIMode::GPT ? "GPT" : "LoRA") << std::endl;
}

#include <sstream>
#include <iomanip>

std::string format_autogen(const std::string& model, const std::string& prompt, const std::string& body) {
    std::ostringstream out;
    out << "// [" << model << "] Completion for: " << prompt << "\n";
    out << "func " << model << "_autogen()\n";
    out << "  log \"AI model: " << model << "\"\n";
    out << "  print \"" << body << "\"\n";
    out << "endfunc\n";
    return out.str();
}

std::string ai_complete(const std::string& prompt) {
    using fusionpp::RuntimeConfig;

    AIMode mode = RuntimeConfig::getInstance().getMode();
    std::string trimmedPrompt = prompt.substr(0, 256); // Optional truncation for safety
    std::string result;

    switch (mode) {
        case AIMode::GPT:
            result = format_autogen("gpt", trimmedPrompt, "Responded with GPT logic");
            break;
        case AIMode::LoRA:
            result = format_autogen("lora", trimmedPrompt, "LoRA-powered synthesis executed");
            break;
        default:
            throw std::runtime_error("Unsupported AIMode during completion dispatch.");
    }

#if FUSIONPP_ENABLE_PROFILING
    std::cerr << "[Completion] " << modeToString(mode)
              << " executed for prompt: \"" << trimmedPrompt << "\"\n";
#endif

    return result;
}


// ====== Input & Action Binding ======
std::unordered_map<std::string, std::function<void(const std::string&)>> inputActions;
std::mutex reloadMutex;
std::string lastScript;

void bind_input(const std::string& key, std::function<void(const std::string&)> handler) {
    inputActions[key] = handler;
    std::cout << "[🎮] Bound input: " << key << std::endl;
}

// ====== Bytecode System ======
struct Bytecode {
    std::string op;
    std::vector<std::string> args;
};
using BytecodeFunc = std::vector<Bytecode>;
std::unordered_map<std::string, BytecodeFunc> bytecodeRegistry;

void load_bytecode(const std::string& file) {
    std::ifstream in(file);
    if (!in) { std::cout << "[❌] Bytecode file not found: " << file << std::endl; return; }
    std::string line, funcName;
    BytecodeFunc current;
    while (std::getline(in, line)) {
        if (line.find("func ") == 0) {
            if (!current.empty() && !funcName.empty())
                bytecodeRegistry[funcName] = current;
            funcName = line.substr(5);
            current.clear();
        } else if (line.find("print") == 0) {
            size_t q = line.find("\"");
            size_t q2 = line.rfind("\"");
            std::string msg = (q != std::string::npos && q2 != q) ? line.substr(q+1, q2-q-1) : "???";
            current.push_back({"print", {msg}});
        } // Add more ops as needed
    }
    if (!current.empty() && !funcName.empty())
        bytecodeRegistry[funcName] = current;
    std::cout << "[💾] Loaded bytecode from: " << file << std::endl;
}

void run_bytecode(const std::string& name) {
    if (!bytecodeRegistry.count(name)) {
        std::cout << "[❌] Bytecode not found: " << name << std::endl;
        return;
    }
    std::cout << "[🚦] Running bytecode: " << name << std::endl;
    for (auto& b : bytecodeRegistry[name]) {
        if (b.op == "print") std::cout << "[PRINT] " << b.args[0] << std::endl;
    }
}

// ====== Multiplayer Input via TCP Socket ======
void multiplayer_input_listener(unsigned short port) {
    try {
        asio::io_context io;
        asio::ip::tcp::acceptor acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        std::cout << "[🔗] Multiplayer input server listening on port " << port << "...\n";
        while (true) {
            asio::ip::tcp::socket socket(io);
            acceptor.accept(socket);
            std::array<char, 256> buf{};
            asio::error_code ec;
            size_t len = socket.read_some(asio::buffer(buf), ec);
            if (!ec && len > 0) {
                std::string msg(buf.data(), len);
                std::cout << "[🌍 Multiplayer] Received: " << msg << std::endl;
                size_t sp = msg.find(' ');
                std::string cmd = sp != std::string::npos ? msg.substr(0, sp) : msg;
                std::string arg = sp != std::string::npos ? msg.substr(sp + 1) : "";
                if (inputActions.count(cmd)) inputActions[cmd](arg);
            }
        }
    } catch (std::exception& e) {
        std::cout << "[❌] Multiplayer listener error: " << e.what() << "\n";
    }
}

// ====== Web Dashboard Hook (Very Basic HTTP) ======
void web_dashboard(unsigned short port) {
    try {
        asio::io_context io;
        asio::ip::tcp::acceptor acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
        std::cout << "[🌐] Web dashboard active on port " << port << "\n";
        while (true) {
            asio::ip::tcp::socket socket(io);
            acceptor.accept(socket);
            std::array<char, 2048> reqbuf{};
            asio::error_code ec;
            size_t len = socket.read_some(asio::buffer(reqbuf), ec);
            std::string response =
                "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                "<html><body><h2>Fusion++ Dashboard</h2>"
                "<pre>";
            std::ifstream log("reinforce_log.json");
            if (log) { std::string l; while (std::getline(log, l)) response += l + "\n"; }
            response += "</pre></body></html>\r\n";
            asio::write(socket, asio::buffer(response), ec);
        }
    } catch (std::exception& e) {
        std::cout << "[❌] Web dashboard error: " << e.what() << "\n";
    }
}

// ====== Script/Log/Reload Bindings ======
void reload_script(const std::string&) {
    std::lock_guard<std::mutex> lock(reloadMutex);
    std::ifstream in("autogen.fpp");
    if (!in) { std::cout << "[❌] autogen.fpp not found.\n"; return; }
    std::string line, content;
    while (std::getline(in, line)) content += line + "\n";
    lastScript = content;
    std::cout << "[🔄] Reloaded script.\n";
}
void show_log(const std::string&) {
    std::ifstream f("reinforce_log.json");
    if (!f) { std::cout << "[❌] No reinforce_log.json.\n"; return; }
    std::string line;
    while (std::getline(f, line)) std::cout << line << "\n";
}

// ====== Main ======
int main() {
    // Input actions (can be triggered by local or network)
    bind_input("reload", reload_script);
    bind_input("toggle_gpt", [](const std::string&) { toggle_ai_mode("gpt"); });
    bind_input("toggle_lora", [](const std::string&) { toggle_ai_mode("lora"); });
    bind_input("run_script", [](const std::string& arg) {
        std::cout << "[RUN] Executing script (raw):\n" << lastScript << std::endl;
    });
    bind_input("run_cutscene", [](const std::string&) {
        std::ifstream in("cinematic.fpp");
        if (!in) { std::cout << "[❌] cinematic.fpp not found.\n"; return; }
        std::string line;
        while (std::getline(in, line)) std::cout << "[🎬] " << line << std::endl;
    });
    bind_input("show_log", show_log);
    bind_input("run_bytecode", [](const std::string& arg) { run_bytecode(arg); });
    bind_input("load_bytecode", [](const std::string& arg) { load_bytecode(arg); });

    // Start multiplayer and dashboard servers
    std::thread(multiplayer_input_listener, 8765).detach();
    std::thread(web_dashboard, 8080).detach();

    std::cout << "Fusion++ Runtime (type commands or connect multiplayer):\n";
    std::string cmd, arg;
    while (true) {
        std::getline(std::cin, cmd);
        if (cmd == "exit") break;
        size_t sp = cmd.find(' ');
        std::string c = sp != std::string::npos ? cmd.substr(0, sp) : cmd;
        std::string a = sp != std::string::npos ? cmd.substr(sp + 1) : "";
        if (inputActions.count(c)) inputActions[c](a);
        else std::cout << "[!] Unknown command.\n";
    }
    std::cout << "[💾] Exiting Fusion++.\n";
    return 0;
}

std::string emit_opcodes(const std::string& model) {
    if (model == "gpt") {
        return "op_push \"GPT\"\nop_log\nop_load \"strategy:gpt.core\"\nop_exec\n";
    } else if (model == "lora") {
        return "op_push \"LoRA\"\nop_log\nop_load \"strategy:lora.passive\"\nop_exec\n";
    }
    return "op_nop\n";
}

std::string format_autogen(const std::string& model, const std::string& prompt, const std::string& body) {
    std::ostringstream out;
    out << "// [" << model << "] Completion for: " << prompt << "\n";
    out << "func " << model << "_autogen()\n";
    out << "  log \"AI model: " << model << "\"\n";
    out << "  opcode_block {\n"
        << emit_opcodes(model)
        << "  }\n";
    out << "  print \"" << body << "\"\n";
    out << "endfunc\n";
    return out.str();
}

void emit_ast_trace(const std::string& funcCode, const std::string& model) {
    std::ofstream out("last_ast.trace.json");
    out << "{\n"
        << "  \"model\": \"" << model << "\",\n"
        << "  \"nodes\": [\n";

    // Simple stub trace
    std::istringstream stream(funcCode);
    std::string line;
    while (std::getline(stream, line)) {
        out << "    {\"type\": \"line\", \"content\": \"" << line << "\"},\n";
    }

    out.seekp(-2, std::ios_base::end); // remove trailing comma
    out << "\n  ]\n}\n";
}

emit_ast_trace(result, model);

std::string write_fpp(const std::string& funcBody, const std::string& model) {
    std::string filename = model + "_autogen.fpp";
    std::ofstream out(filename);
    out << funcBody;
    return filename;
}

std::string result = format_autogen(...);
emit_ast_trace(result, model);
write_fpp(result, model);
return result;

// fusionpp_bytecode_compiler.h
#pragma once
#include "fusionpp_fpp_parser.h"
#include <unordered_map>
#include <vector>
#include <string>

struct Bytecode { std::string op; std::vector<std::string> args; };
using BytecodeFunc = std::vector<Bytecode>;

class FppBytecodeCompiler {
public:
    std::unordered_map<std::string, BytecodeFunc> compile(const std::vector<std::shared_ptr<FppNode>>& nodes) {
        std::unordered_map<std::string, BytecodeFunc> out;
        for (const auto& n : nodes) {
            if (n->type == NodeType::Func) {
                BytecodeFunc func;
                for (const auto& b : n->body) {
                    if (b->type == NodeType::Print) func.push_back({"print", {b->name}});
                    if (b->type == NodeType::Let) func.push_back({"let", {b->name, b->args[0]}});
                }
                out[n->name] = func;
            }
        }
        return out;
    }
};

#include "fusionpp_fpp_parser.h"
#include "fusionpp_bytecode_compiler.h"

// Load and inject at runtime
void inject_fpp(const std::string& fpp_path) {
    FppParser parser;
    auto ast = parser.parse(fpp_path);

    FppBytecodeCompiler compiler;
    auto bytecode_map = compiler.compile(ast);

    // Register all functions dynamically
    for (auto& [k, v] : bytecode_map)
        bytecodeRegistry[k] = v;

    std::cout << "[🔗] Injected bytecode functions from: " << fpp_path << "\n";
}
