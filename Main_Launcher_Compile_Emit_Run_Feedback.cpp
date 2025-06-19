#include "fpp_parser.h"
#include "ir_emitter.h"
#include "fpp_ast.h"
#include "fusion_runtime.h"

int main() {
    FPPParser parser;
    auto nodes = parser.parse("main.fpp");

    IRCompiler irc;
    BytecodeStream bytecode = irc.emitBytecode(nodes);

    FusionRuntime engine;
    engine.execute(bytecode);
    engine.showEventStats();

    std::cout << "Live JIT feedback written to jit_feedback.fpp\n";
    return 0;
}

// e.g., from HTTP dashboard POST or TCP message: "autorun_fpp autogen.fpp"
if (cmd == "autorun_fpp") {
    inject_fpp(arg);           // Parses, compiles, injects bytecode
    run_bytecode("main");      // Auto-runs function "main" or your entrypoint
}
