#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <map>

std::map<std::string, int> simState;

void simulateLine(const std::string& line) {
    if (line.find("let") != std::string::npos) {
        std::string var = line.substr(line.find("let") + 4, line.find("=") - 5);
        int val = std::stoi(line.substr(line.find("=") + 1));
        simState[var] = val;
        std::cout << "🧪 Let " << var << " = " << val << "\n";
    }
    if (line.find("print") != std::string::npos) {
        std::string msg = line.substr(line.find("\"")+1);
        msg = msg.substr(0, msg.find("\""));
        std::cout << "📢 " << msg << "\n";
    }
    if (line.find("camera move_to") != std::string::npos)
        std::cout << "🎥 Sim camera move triggered.\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void runSimulation(const std::string& file) {
    std::ifstream f(file);
    std::string line;
    while (std::getline(f, line))
        simulateLine(line);

    std::cout << "\n📊 Simulation complete.\nState:\n";
    for (auto& [k, v] : simState)
        std::cout << " - " << k << " = " << v << "\n";
}
