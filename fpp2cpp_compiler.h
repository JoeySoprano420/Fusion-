#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <filesystem>

// === Fusion++ .fpp → .cpp Compiler ===
class FusionFPPCompiler {
    std::string inputPath;
    std::string outputCPPPath;

    std::map<std::string, std::string> typeMap = {
        {"int", "int"},
        {"string", "std::string"},
        {"float", "float"},
        {"bool", "bool"}
    };

    std::string transformLine(const std::string& line) {
        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "let" || token == "init") {
            std::string name, eq, value;
            ss >> name >> eq >> value;
            return "    auto " + name + " = " + value + ";";
        }

        if (token == "define") {
            std::string typename_, varname;
            ss >> typename_ >> varname;
            return "    " + typeMap[typename_] + " " + varname + ";";
        }

        if (token == "instruction") {
            std::string name;
            ss >> name;
            return "void " + name + "() {";
        }

        if (token == "wrap") {
            return "} // end wrap";
        }

        if (token == "print") {
            std::string msg;
            std::getline(ss, msg);
            return "    std::cout << " + msg + " << std::endl;";
        }

        if (token == "emit") {
            return "// [EMIT] No-op for now";
        }

        return "// Unrecognized: " + line;
    }

public:
    FusionFPPCompiler(const std::string& inPath, const std::string& outPath)
        : inputPath(inPath), outputCPPPath(outPath) {}

    void compileToCPP() {
        std::ifstream in(inputPath);
        std::ofstream out(outputCPPPath);

        out << "#include <iostream>\n\nint main() {\n";

        std::string line;
        while (std::getline(in, line)) {
            if (line.empty()) continue;
            std::string cppLine = transformLine(line);
            out << cppLine << "\n";
        }

        out << "    return 0;\n}\n";

        in.close();
        out.close();

        std::cout << "[✔] Emitted C++ code to: " << outputCPPPath << "\n";
    }

    void compileToEXE(const std::string& exeName = "a.exe") {
        std::string cmd = "g++ " + outputCPPPath + " -o " + exeName + " -lssl -lcrypto";
        int result = std::system(cmd.c_str());
        if (result == 0)
            std::cout << "[🎮] Compiled to executable: " << exeName << "\n";
        else
            std::cerr << "[❌] Compilation failed.\n";
    }
};
