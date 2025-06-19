#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

using FusionFunction = std::function<void()>;

class RuntimeRegistry {
    std::unordered_map<std::string, FusionFunction> functionTable;
    std::unordered_map<std::string, std::filesystem::file_time_type> lastModified;

public:
    void registerFunction(const std::string& name, FusionFunction fn) {
        functionTable[name] = fn;
        std::cout << "[Runtime] Registered: " << name << "\n";
    }

    void callFunction(const std::string& name) {
        if (functionTable.count(name)) {
            std::cout << "[Runtime] Executing: " << name << "\n";
            functionTable[name]();
        } else {
            std::cerr << "[Runtime] Function not found: " << name << "\n";
        }
    }

    void loadDynamicFile(const std::string& path) {
        auto lastTime = std::filesystem::last_write_time(path);
        if (lastModified[path] != lastTime) {
            std::ifstream f(path);
            std::string line;
            std::ostringstream body;
            while (std::getline(f, line)) body << line << "\n";
            std::string code = body.str();

            // Simulate execution
            std::cout << "[HotPatch] Re-executing code in: " << path << "\n";
            std::cout << code << "\n";

            lastModified[path] = lastTime;
        }
    }

    void hotReloadWatchLoop(const std::vector<std::string>& watched) {
        std::cout << "[HotPatch] Watching for changes...\n";
        while (true) {
            for (const auto& path : watched)
                loadDynamicFile(path);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
};

struct BytecodeFunction {
    std::string name;
    std::vector<Bytecode> code;
};

class FusionRuntime {
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, BytecodeFunction> byteFuncs;

public:
    void loadFunction(const BytecodeFunction& fn) {
        byteFuncs[fn.name] = fn;
        std::cout << "[Bytecode] Loaded: " << fn.name << " (" << fn.code.size() << " ops)\n";
    }

    void reloadFunctionFromFile(const std::string& fname, FPPParser& parser, IRCompiler& irc) {
        auto nodes = parser.parse(fname);
        auto bytecode = irc.emitBytecode(nodes);
        loadFunction({fname, bytecode.code});
    }

    void callByteFunc(const std::string& name) {
        if (!byteFuncs.count(name)) {
            std::cerr << "[Runtime] No bytecode for: " << name << "\n";
            return;
        }
        std::cout << "[Bytecode] Running: " << name << "\n";
        execute(BytecodeStream{byteFuncs[name].code});
    }

    void execute(const BytecodeStream& stream) {
        for (auto& inst : stream.code) {
            if (inst.op == OpCode::LOAD_CONST) variables[inst.args[0]] = inst.args[1];
            else if (inst.op == OpCode::PRINT) std::cout << variables[inst.args[0]] << "\n";
            else if (inst.op == OpCode::CALL) callByteFunc(inst.args[0]);
            else if (inst.op == OpCode::END) break;
        }
    }
};

