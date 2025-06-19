#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

class FusionRuntime {
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, int> callCounts;
public:
    // Feedback buffer: holds .fpp lines to write back based on runtime
    std::vector<std::string> jitFeedbackLines;

    void execute(const BytecodeStream& bytecode) {
        for (size_t i = 0; i < bytecode.code.size(); ++i) {
            const auto& inst = bytecode.code[i];
            if (inst.op == OpCode::LOAD_CONST) {
                variables[inst.args[0]] = inst.args[1];
            } else if (inst.op == OpCode::PRINT) {
                std::cout << variables.count(inst.args[0]) ? variables[inst.args[0]] : inst.args[0] << std::endl;
                // Live JIT: capture hot prints
                jitFeedbackLines.push_back("print " + (variables.count(inst.args[0]) ? variables[inst.args[0]] : inst.args[0]));
            } else if (inst.op == OpCode::CALL) {
                callCounts[inst.args[0]]++;
            } else if (inst.op == OpCode::END) {
                break;
            }
        }
        // Live JIT: Write out hot feedback to .fpp
        std::ofstream jitOut("jit_feedback.fpp");
        for (const auto& l : jitFeedbackLines) jitOut << l << "\n";
        jitOut.close();
    }

    void showEventStats() {
        for (const auto& [name, count] : callCounts)
            std::cout << "[JIT] Function " << name << " called " << count << " times\n";
    }
};
