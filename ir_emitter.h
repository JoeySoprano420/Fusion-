#pragma once
#include "fpp_ast.h"
#include <fstream>

class IRCompiler {
public:
    void emitCPP(const std::vector<std::shared_ptr<ASTNode>>& nodes, const std::string& outPath) {
        std::ofstream out(outPath);
        out << "#include <iostream>\n\n";

        for (const auto& node : nodes) {
            switch (node->type) {
            case ASTType::Function:
                out << "void " << node->name << "() {\n";
                for (const auto& b : node->body) {
                    if (b->type == ASTType::Print)
                        out << "  std::cout << " << b->name << " << std::endl;\n";
                }
                out << "}\n";
                break;

            case ASTType::Let:
                out << "auto " << node->name << " = " << node->args[0] << ";\n";
                break;

            case ASTType::Define:
                out << node->args[0] << " " << node->name << ";\n";
                break;

            case ASTType::Print:
                out << "std::cout << " << node->name << " << std::endl;\n";
                break;

            default:
                break;
            }
        }

        out << "\nint main() {\n";
        for (const auto& node : nodes) {
            if (node->type == ASTType::Function)
                out << "  " << node->name << "();\n";
        }
        out << "  return 0;\n}\n";
    }
};
