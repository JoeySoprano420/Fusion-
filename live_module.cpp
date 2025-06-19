#include "fusion_runtime.h"

void registerLive(RuntimeRegistry& reg) {
    reg.registerFunction("intro", []() {
        std::cout << ">> Running intro from registered engine function.\n";
    });

    reg.registerFunction("mission_start", []() {
        std::cout << ">> Mission start initialized.\n";
    });
}
