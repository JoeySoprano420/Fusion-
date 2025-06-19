#include "fpp2cpp_compiler.h"

int main() {
    std::string sourceFile = "example.fpp";
    std::string cppFile = "example_gen.cpp";
    std::string exeFile = "example.exe";

    FusionFPPCompiler compiler(sourceFile, cppFile);
    compiler.compileToCPP();
    compiler.compileToEXE(exeFile);

    return 0;
}
