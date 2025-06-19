#include "fusion_runtime.h"

extern void registerLive(RuntimeRegistry&);

int main() {
    RuntimeRegistry registry;
    registerLive(registry);

    registry.callFunction("intro");
    registry.callFunction("mission_start");

    std::vector<std::string> watchList = { "dynamic_patch.fpp" };
    registry.hotReloadWatchLoop(watchList);

    return 0;
}
