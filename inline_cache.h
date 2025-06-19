#pragma once
#include <unordered_map>
#include <string>

class InlineFunctionCache {
    std::unordered_map<std::string, std::string> cache;

public:
    std::string get(const std::string& name) {
        if (cache.count(name)) return cache[name];
        if (name == "draw_hero") {
            cache[name] = "std::cout << \"Hero drawn!\" << std::endl;";
            return cache[name];
        }
        return "// inline not found";
    }

    void define(const std::string& name, const std::string& code) {
        cache[name] = code;
    }
};
