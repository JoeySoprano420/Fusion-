#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <nlohmann/json.hpp> // JSON library
using namespace std;
using json = nlohmann::json;

class OfflineFppAI {
    map<string,string> intentMap;
    string dbFile = "offline_ai_memory.json";

public:
    OfflineFppAI() { load(); }

    void load() {
        ifstream f(dbFile);
        if (f) {
            json j;
            f >> j;
            for (auto& [k, v] : j.items())
                intentMap[k] = v;
            cout << "[💾] Offline AI memory loaded.\n";
        }
    }

    void save() {
        json j;
        for (auto& [k, v] : intentMap)
            j[k] = v;
        ofstream f(dbFile);
        f << j.dump(2);
        cout << "[📁] Memory saved.\n";
    }

    string suggest(const string& intent) {
        for (auto& [k, v] : intentMap)
            if (intent.find(k) != string::npos)
                return v;
        return "// No match found for intent: " + intent + "\n";
    }

    void teach(const string& intent, const string& code) {
        intentMap[intent] = code;
        save();
    }
};
