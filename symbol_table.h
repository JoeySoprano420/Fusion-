#pragma once
#include <unordered_map>
#include <vector>
#include <string>

class SymbolTable {
    std::unordered_map<std::string, int> table;
    std::vector<std::string> compressed;
    int nextId = 0;
public:
    int intern(const std::string& name) {
        if (table.find(name) == table.end()) {
            table[name] = nextId++;
            compressed.push_back(name);
        }
        return table[name];
    }

    std::string getSymbol(int id) const {
        return id >= 0 && id < compressed.size() ? compressed[id] : "";
    }

    int size() const { return compressed.size(); }
};
