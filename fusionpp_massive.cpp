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
