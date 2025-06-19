#include <map>
#include <string>
#include <iostream>

class DSLIntentMapper {
    std::map<std::string, std::string> patterns;

public:
    DSLIntentMapper() {
        patterns["intro cinematic"] =
R"(func play_intro
    camera move_to 0 2 8 over 4s
    camera focus entity_player
    camera shake 0.3 over 2s
    print "The battle begins..."
endfunc
)";
        patterns["heal player"] =
R"(func heal_player
    let health = 100
    print "Health fully restored."
endfunc
)";
    }

    std::string resolve(const std::string& intent) {
        for (auto& [k, v] : patterns)
            if (intent.find(k) != std::string::npos)
                return v;
        return "func undefined_intent\n    print \"No match.\"\nendfunc\n";
    }

    void teach(const std::string& phrase, const std::string& script) {
        patterns[phrase] = script;
        std::cout << "[DSL] Learned new mapping for: " << phrase << "\n";
    }
};
