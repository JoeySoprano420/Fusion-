#include "completion_engine.h"
#include "net_sync.h"
#include "cinematic_engine.h"

int main() {
    // Completion
    FPPCompletion autoComp;
    autoComp.learnFromFile("main.fpp");
    auto suggestions = autoComp.suggest("ca");
    for (auto& s : suggestions) std::cout << "Suggest: " << s << "\n";

    // Network Sync
    FusionNetSync sync;
    sync.setState("player_health", "100");
    sync.setState("position", "x:5,y:2,z:0");
    sync.startSyncLoop();

    // Cinematics
    CinematicSystem cam;
    cam.loadFromScript("cinematic.fpp");

    // Loop placeholder
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
