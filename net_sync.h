#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>

class FusionNetSync {
    std::unordered_map<std::string, std::string> gameState;
    std::mutex stateMutex;

public:
    void setState(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(stateMutex);
        gameState[key] = value;
    }

    void broadcastState() {
        std::lock_guard<std::mutex> lock(stateMutex);
        std::cout << "[WebSocket] Broadcasting game state:\n";
        for (auto& [k, v] : gameState)
            std::cout << "  " << k << ": " << v << "\n";
    }

    void startSyncLoop() {
        std::thread([this]() {
            while (true) {
                broadcastState();
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }).detach();
    }
};
