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
