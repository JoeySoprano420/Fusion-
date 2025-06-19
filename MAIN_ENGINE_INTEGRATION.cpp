int main() {
    FPPParser parser;
    IRCompiler irc;
    FusionRuntime runtime;
    InputManager input;
    MemorySnapshot snapshot;

    runtime.reloadFunctionFromFile("mission.fpp", parser, irc);

    input.bind("start", [&]() {
        runtime.callByteFunc("mission.fpp");
        snapshot.takeSnapshot(runtime.variables);  // Save state after start
    });

    input.bind("reload", [&]() {
        runtime.reloadFunctionFromFile("mission.fpp", parser, irc);
        std::cout << "[Runtime] Reloaded mission.fpp\n";
    });

    input.bind("restore", [&]() {
        snapshot.restoreSnapshot(runtime.variables);
    });

    input.listenLoop();
    return 0;
}
