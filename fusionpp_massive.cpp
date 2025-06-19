#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <thread>
#include <map>
#include <set>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cstring>
#include <functional>
#include <netinet/in.h>
#include <unistd.h>

// Entity base types
struct Vector2 { float x=0, y=0; Vector2 operator+(const Vector2& o) const { return {x+o.x, y+o.y}; } };

// Physics body
struct PhysicsBody {
    Vector2 position, velocity, force;
    float mass = 1.0f;
    bool isStatic = false;
    void applyForce(const Vector2& f) { if (!isStatic) force = force + f; }
    void integrate(float dt) {
        if (isStatic) return;
        Vector2 a = { force.x / mass, force.y / mass };
        velocity = velocity + a * dt;
        position = position + velocity * dt;
        force = {0,0};
    }
};

// Physics Engine
class PhysicsEngine {
public:
    std::vector<PhysicsBody*> bodies;
    void addBody(PhysicsBody* b) { bodies.push_back(b); }
    void step(float dt) { for (auto* b : bodies) b->integrate(dt); }
};

// Multiplayer
class MultiplayerServer {
    int server_fd, new_socket;
    struct sockaddr_in address;
    std::set<int> clients;
    std::mutex mtx;
public:
    void start(int port = 8080) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        bind(server_fd, (struct sockaddr*)&address, sizeof(address));
        listen(server_fd, 3);
        std::thread(&MultiplayerServer::acceptClients, this).detach();
    }
    void acceptClients() {
        socklen_t len = sizeof(address);
        while (true) {
            new_socket = accept(server_fd, (struct sockaddr*)&address, &len);
            std::lock_guard<std::mutex> lock(mtx);
            clients.insert(new_socket);
            std::thread(&MultiplayerServer::handleClient, this, new_socket).detach();
        }
    }
    void handleClient(int fd) {
        char buf[1024];
        while (read(fd, buf, 1024) > 0)
            broadcast(std::string(buf), fd);
        std::lock_guard<std::mutex> lock(mtx);
        clients.erase(fd);
        close(fd);
    }
    void broadcast(const std::string& msg, int exclude = -1) {
        std::lock_guard<std::mutex> lock(mtx);
        for (int c : clients)
            if (c != exclude)
                send(c, msg.c_str(), msg.size(), 0);
    }
};

// AI Behaviors
enum class AIType { Idle, Patrol, Chase, Flee, Ambush };
struct AIComponent {
    AIType type;
    Vector2 target;
    void update(PhysicsBody& body) {
        if (type == AIType::Patrol) body.velocity.x = (body.position.x < 5 ? 0.1f : -0.1f);
        else if (type == AIType::Chase) {
            Vector2 d = {target.x - body.position.x, target.y - body.position.y};
            float len = std::sqrt(d.x*d.x + d.y*d.y);
            if (len > 0) body.applyForce({d.x/len, d.y/len});
        }
    }
};

// FusionScript Compiler + VM
enum class OpCode { SAY, WAIT, EXIT };
struct Bytecode { OpCode op; std::vector<std::string> args; };

class FusionScriptCompiler {
public:
    static std::vector<Bytecode> compile(const std::string& src) {
        std::istringstream ss(src);
        std::vector<Bytecode> code;
        std::string line;
        while (std::getline(ss, line)) {
            std::istringstream ls(line);
            std::string cmd;
            ls >> cmd;
            Bytecode b;
            if (cmd == "say") b.op = OpCode::SAY;
            else if (cmd == "wait") b.op = OpCode::WAIT;
            else b.op = OpCode::EXIT;
            std::string arg;
            while (ls >> arg) b.args.push_back(arg);
            code.push_back(b);
        }
        return code;
    }
};

class FusionBytecodeVM {
    std::vector<Bytecode> prog; size_t ip = 0;
public:
    FusionBytecodeVM(const std::vector<Bytecode>& p) : prog(p) {}
    void run() {
        while (ip < prog.size()) {
            auto& b = prog[ip];
            if (b.op == OpCode::SAY)
                for (auto& s : b.args) std::cout << s << " ";
            else if (b.op == OpCode::WAIT)
                std::this_thread::sleep_for(std::chrono::milliseconds(std::stoi(b.args[0])));
            else return;
            std::cout << std::endl;
            ip++;
        }
    }
};

// Terrain + UI + Camera omitted in this excerpt

int main(int argc, char** argv) {
    std::cout << "Fusion++ Engine Runtime Launching...\n";
    PhysicsEngine physics;
    MultiplayerServer server;
    server.start(9001);

    PhysicsBody p1, p2;
    p2.position = {5, 0};
    physics.addBody(&p1);
    physics.addBody(&p2);

    AIComponent ai = {AIType::Chase, {0,0}};

    std::thread([&](){
        while (true) {
            physics.step(0.016f);
            ai.update(p2);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }).detach();

    std::string script = "say Hello Fusion++\\nwait 1000\\nsay Launching Mission\\nexit";
    FusionBytecodeVM(FusionScriptCompiler::compile(script)).run();

    return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <cmath>
#include <chrono>
#include <functional>
#include <netinet/in.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ———————————————————————————————————————
// Compiler AI Learning Engine
// ———————————————————————————————————————
struct CompilerPattern {
    std::string patternName;
    int appearances = 0;
    int user_accepts = 0;
    int user_rejects = 0;
    std::string generatedCode;

    void registerUse(bool accepted) {
        appearances++;
        if (accepted) user_accepts++;
        else user_rejects++;
    }

    float reliability() const {
        return (user_accepts + 1.0f) / (user_rejects + 1.0f);
    }
};

class CompilerAI {
    std::string dataFile = "fusion_compiler_ai.json";
    std::map<std::string, CompilerPattern> learnedPatterns;

public:
    CompilerAI() { load(); }

    void load() {
        std::ifstream f(dataFile);
        if (!f.is_open()) return;
        json j; f >> j;
        for (auto& el : j.items()) {
            CompilerPattern p;
            p.patternName = el.key();
            p.appearances = el.value()["appearances"];
            p.user_accepts = el.value()["user_accepts"];
            p.user_rejects = el.value()["user_rejects"];
            p.generatedCode = el.value()["generatedCode"];
            learnedPatterns[p.patternName] = p;
        }
    }

    void save() {
        json j;
        for (auto& [k, v] : learnedPatterns) {
            j[k] = {
                {"appearances", v.appearances},
                {"user_accepts", v.user_accepts},
                {"user_rejects", v.user_rejects},
                {"generatedCode", v.generatedCode}
            };
        }
        std::ofstream out(dataFile);
        out << j.dump(4);
    }

    void observePattern(const std::string& name, const std::string& code, bool accepted) {
        if (!learnedPatterns.count(name))
            learnedPatterns[name] = CompilerPattern{name, 0, 0, 0, code};
        learnedPatterns[name].registerUse(accepted);
        save();
    }

    std::string suggestEnhancement(const std::string& context) {
        float bestScore = 0;
        std::string bestCode;
        for (const auto& [name, p] : learnedPatterns) {
            if (context.find(name) != std::string::npos && p.reliability() > bestScore) {
                bestScore = p.reliability();
                bestCode = p.generatedCode;
            }
        }
        return bestCode;
    }

    void trainFromCodebase(const std::vector<std::string>& lines) {
        for (const auto& line : lines) {
            if (line.find("instruction") != std::string::npos) {
                observePattern("instruction", "func auto_generated() { /*...*/ }", true);
            } else if (line.find("ai") != std::string::npos) {
                observePattern("ai", "define new_ai derives Adaptive", true);
            }
        }
    }
};

// ———————————————————————————————————————
// Minimal runtime example
// ———————————————————————————————————————
int main() {
    std::cout << "🧠 Fusion++ AI Compiler Booting Up...\n";

    CompilerAI ai;
    ai.observePattern("loop", "loop { print(\"adaptive loop\") }", true);
    ai.observePattern("ai", "define smart_ai derives Reflexive", true);

    std::string context = "ai loop patrol";
    std::string suggestion = ai.suggestEnhancement(context);

    if (!suggestion.empty()) {
        std::cout << "\n✨ AI Compiler Suggestion:\n" << suggestion << "\n";
    } else {
        std::cout << "\n(No suggestion available for context: " << context << ")\n";
    }

    return 0;
}

// ==== AUTO-INJECT, LIVE-TRAINING, & DASHBOARD ====
#include <filesystem>
namespace fs = std::filesystem;

class FusionCompiler {
    CompilerAI compilerAI;

public:
    void trainFromFolder(const std::string& folderPath) {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.path().extension() == ".fpp") {
                std::ifstream file(entry.path());
                if (!file.is_open()) continue;
                std::vector<std::string> lines;
                std::string line;
                while (std::getline(file, line)) lines.push_back(line);
                compilerAI.trainFromCodebase(lines);
            }
        }
        std::cout << "📘 Trained AI from all .fpp files in " << folderPath << "\n";
    }

    std::string generateCodeWithAI(const std::string& context) {
        std::string suggestion = compilerAI.suggestEnhancement(context);
        if (!suggestion.empty()) {
            std::cout << "💡 Injecting AI-enhanced code: " << suggestion << "\n";
            return suggestion;
        }
        return "// no suggestion from AI\n";
    }

    void showLearningDashboard() {
        std::ifstream in("fusion_compiler_ai.json");
        if (!in.is_open()) {
            std::cout << "⚠️ No AI learning data available.\n";
            return;
        }
        json j; in >> j;
        std::cout << "\n===== 🧠 Fusion++ AI Pattern Dashboard =====\n";
        for (auto& [key, val] : j.items()) {
            std::cout << "🔹 Pattern: " << key << "\n";
            std::cout << "   Uses     : " << val["appearances"] << "\n";
            std::cout << "   Success  : " << val["user_accepts"] << "\n";
            std::cout << "   Failures : " << val["user_rejects"] << "\n";
            std::cout << "   Reliability Score: "
                      << ((val["user_accepts"].get<float>() + 1.0f) / (val["user_rejects"].get<float>() + 1.0f))
                      << "\n";
            std::cout << "   Suggested Code: " << val["generatedCode"] << "\n\n";
        }
    }
};

// === Enhanced Compiler Entry Point ===
int main() {
    std::cout << "\n🔥 Fusion++ Self-Learning Compiler\n";

    FusionCompiler fusion;
    fusion.trainFromFolder("./examples");

    std::string injected = fusion.generateCodeWithAI("ai patrol loop");
    std::cout << "\n📤 Final Injected Code:\n" << injected << "\n";

    fusion.showLearningDashboard();

    return 0;
}

