#include <fstream>
#include <chrono>
#include "include/json.hpp"
using namespace std;
using json = nlohmann::json;

void log_fpp_run(string code, bool success, int durationMs) {
    json log = {
        {"timestamp", chrono::system_clock::to_time_t(chrono::system_clock::now())},
        {"code", code},
        {"result", success ? "success" : "fail"},
        {"metrics", {{"execution_time_ms", durationMs}}}
    };
    ofstream file("reinforce_log.json", ios::app);
    file << log.dump() << "\n";
}

int run_fpp(string fpp_code) {
    auto start = chrono::high_resolution_clock::now();

    // Placeholder: Simulate success or failure
    bool success = fpp_code.find("fail") == string::npos;

    auto end = chrono::high_resolution_clock::now();
    int duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    log_fpp_run(fpp_code, success, duration);
    return success ? 0 : 1;
}
