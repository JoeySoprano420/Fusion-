#pragma once
#include <string>
#include <iostream>
#include <thread>

class CinematicSystem {
public:
    void executeCommand(const std::string& line) {
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;
        if (cmd == "camera") {
            std::string action;
            ss >> action;
            if (action == "move_to") {
                float x, y, z;
                std::string over, duration;
                ss >> x >> y >> z >> over >> duration;
                std::cout << "[Camera] Moving to (" << x << "," << y << "," << z << ") over " << duration << "\n";
            } else if (action == "focus") {
                std::string target;
                ss >> target;
                std::cout << "[Camera] Focusing on " << target << "\n";
            } else if (action == "shake") {
                float intensity;
                std::string over, duration;
                ss >> intensity >> over >> duration;
                std::cout << "[Camera] Shaking with intensity " << intensity << " over " << duration << "\n";
            }
        }
    }

    void loadFromScript(const std::string& path) {
        std::ifstream f(path);
        std::string line;
        while (std::getline(f, line))
            executeCommand(line);
    }
};
