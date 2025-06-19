#include "fpp_parser.h"
#include "ir_emitter.h"

int main() {
    FPPParser parser;
    auto nodes = parser.parse("main.fpp");

    IRCompiler ir;
    ir.emitCPP(nodes, "main.cpp");

    std::system("g++ main.cpp -o main.exe -lssl -lcrypto");
    std::cout << "✅ Compiled: main.exe\n";
    return 0;
}
