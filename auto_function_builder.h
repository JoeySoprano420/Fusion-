#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <regex>
#include <filesystem>

class FPPFunctionBuilder {
    std::vector<std::string> knownCommands = {
        "print", "let", "define", "load", "stream", "trigger", "camera", "emit", "patch"
    };

    std::map<std::string, std::string> semanticSnippets = {
        {"start mission", 
R"(func mission_start
    print "Mission has begun!"
    let lives = 3
    let score = 0
    camera move_to 0 2 5 over 2s
    emit mission_ready
endfunc
)"},
        {"play intro cutscene", 
R"(func intro_cutscene
    print "Loading intro..."
    camera move_to 0 0 10 over 3s
    camera focus entity_hero
    camera shake 0.2 over 1.5s
endfunc
)"},
        {"set player health", 
R"(func set_player_health
    let health = 100
    print "Health set."
endfunc
)"}
    };

public:
    std::string generateFunction(const std::string& intent) {
        for (auto& [key, snippet] : semanticSnippets) {
            if (intent.find(key) != std::string::npos) {
                std::cout << "[🧠 AutoFunction] Match: " << key << "\n";
                return snippet;
            }
        }

        return "func unknown_function\n    print \"[AutoGen] Unrecognized request.\"\nendfunc\n";
    }

    void injectFunctionToFile(const std::string& path, const std::string& funcCode) {
        std::ofstream f(path, std::ios::app);
        f << "\n" << funcCode;
        std::cout << "[💾] Injected function into: " << path << "\n";
    }
};
