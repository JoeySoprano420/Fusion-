#pragma once
#include <vector>
#include <string>
#include <sstream>

struct SSAInstruction {
    std::string lhs;
    std::string rhs;
};

class SSAConverter {
public:
    std::vector<SSAInstruction> convert(const std::string& baseVar, const std::vector<std::string>& ops) {
        std::vector<SSAInstruction> result;
        std::string prev = baseVar;
        for (size_t i = 0; i < ops.size(); ++i) {
            std::string next = baseVar + "_" + std::to_string(i + 1);
            result.push_back({next, prev + " " + ops[i]});
            prev = next;
        }
        return result;
    }

    std::string dump(const std::vector<SSAInstruction>& ssa) {
        std::ostringstream out;
        for (const auto& inst : ssa)
            out << inst.lhs << " = " << inst.rhs << ";\n";
        return out.str();
    }
};
