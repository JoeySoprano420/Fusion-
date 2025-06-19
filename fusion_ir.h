#pragma once
#include <string>
#include <vector>
#include <sstream>

struct IRInstruction {
    std::string op;
    std::vector<std::string> args;
};

class FusionIR {
    std::vector<IRInstruction> instructions;

public:
    void emit(const std::string& op, const std::vector<std::string>& args) {
        instructions.push_back({op, args});
    }

    std::string dump() const {
        std::ostringstream out;
        for (const auto& inst : instructions) {
            out << inst.op;
            for (const auto& arg : inst.args)
                out << " " << arg;
            out << "\n";
        }
        return out.str();
    }
};
