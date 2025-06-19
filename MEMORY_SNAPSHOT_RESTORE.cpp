class MemorySnapshot {
    std::unordered_map<std::string, std::string> snapshot;

public:
    void takeSnapshot(const std::unordered_map<std::string, std::string>& vars) {
        snapshot = vars;
        std::cout << "[🧬 Snapshot] Saved.\n";
    }

    void restoreSnapshot(std::unordered_map<std::string, std::string>& vars) {
        vars = snapshot;
        std::cout << "[🧬 Snapshot] Restored.\n";
    }
};
