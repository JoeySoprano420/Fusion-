#include <iostream>
#include <string>
#include <map>
using namespace std;

// A minimal offline function suggestion system
class OfflineFppAI {
    map<string,string> intentMap;

public:
    OfflineFppAI() {
        intentMap["spawn enemies"] =
R"(func spawn_wave
    let count = 5
    print "Spawning wave of enemies"
endfunc
)";
        intentMap["heal player"] =
R"(func heal_player
    let health = 100
    print "Player healed"
endfunc
)";
    }

    void teach(const string& intent, const string& code) {
        intentMap[intent] = code;
        cout << "[📚] Learned offline intent: " << intent << endl;
    }

    string suggest(const string& intent) {
        for (auto& [k, v] : intentMap) {
            if (intent.find(k) != string::npos) {
                cout << "[🤖] Matched intent: " << k << endl;
                return v;
            }
        }
        return "// No match found for intent: " + intent + "\n";
    }
};

int main() {
    OfflineFppAI ai;

    while (true) {
        cout << "\nEnter your intent (or type 'exit'): ";
        string intent;
        getline(cin, intent);
        if (intent == "exit") break;

        string suggestion = ai.suggest(intent);
        cout << "\n👉 Suggested .fpp function:\n" << suggestion << endl;

        cout << "✅ Accept this suggestion? (yes/no): ";
        string resp;
        getline(cin, resp);
        if (resp == "yes" || resp == "y") {
            ai.teach(intent, suggestion);
            cout << "[👍] Learned and saved.\n";
        } else {
            cout << "[👎] Suggestion rejected.\n";
        }
    }

    return 0;
}
