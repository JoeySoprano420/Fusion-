#pragma once
#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>

class FPPCompletion {
    std::set<std::string> knownKeywords = {
        "func", "print", "let", "define", "load", "stream",
        "init", "emit", "wrap", "patch", "asm", "trigger"
    };

    std::set<std::string> userSymbols;

public:
    void learnFromFile(const std::string& path) {
        std::ifstream f(path);
        std::string line, token;
        while (std::getline(f, line)) {
            std::istringstream ss(line);
            while (ss >> token) {
                if (token != "func" && token != "let" && token != "define" &&
                    token != "print" && token.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == std::string::npos)
                    userSymbols.insert(token);
            }
        }
    }

    std::vector<std::string> suggest(const std::string& prefix) {
        std::vector<std::string> result;
        for (auto& kw : knownKeywords)
            if (kw.find(prefix) == 0)
                result.push_back(kw);
        for (auto& sym : userSymbols)
            if (sym.find(prefix) == 0)
                result.push_back(sym);
        std::sort(result.begin(), result.end());
        return result;
    }
};
