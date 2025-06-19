#include "symbol_table.h"
#include "inline_cache.h"
#include "ssa.h"
#include "fusion_ir.h"
#include "shader_support.h"
#include "timing_utils.h"
#include "encryption_utils.h"

int main() {
    SymbolTable symtab;
    InlineFunctionCache cache;
    SSAConverter ssaGen;
    FusionIR ir;

    int id = symtab.intern("player");
    std::cout << "[Symbol] Registered ID for 'player': " << id << "\n";

    std::cout << "[Inline] Expansion:\n" << cache.get("draw_hero") << "\n";

    std::vector<std::string> ops = {"+ 1", "* 2", "- 3"};
    auto ssa = ssaGen.convert("x", ops);
    std::cout << "[SSA] Generated:\n" << ssaGen.dump(ssa) << "\n";

    ir.emit("load", {"r1", "10"});
    ir.emit("add", {"r1", "r2"});
    std::cout << "[IR] Dump:\n" << ir.dump() << "\n";

    loadShader("terrain_shader");

    timed("Loop Unroll", []() {
        for (int i = 0; i < 4; ++i)
            std::cout << ">> Step " << i << "\n";
    });

    std::string hash = sha256("Fusion++ Secret");
    std::cout << "[Hash] SHA256 = " << hash << "\n";

    return 0;
}

#include "include/json.hpp"
using json = nlohmann::json;

