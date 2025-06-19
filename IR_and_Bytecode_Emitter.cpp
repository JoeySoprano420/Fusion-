#pragma once
#include "fpp_ast.h"
#include <fstream>

class IRCompiler {
public:
    BytecodeStream emitBytecode(const std::vector<std::shared_ptr<ASTNode>>& nodes) {
        BytecodeStream stream;
        for (const auto& node : nodes) {
            switch (node->type) {
            case ASTType::Let:
                stream.emit(OpCode::LOAD_CONST, {node->name, node->args[0]});
                break;
            case ASTType::Print:
                stream.emit(OpCode::PRINT, {node->name});
                break;
            case ASTType::Function:
                stream.emit(OpCode::CALL, {node->name});
                for (const auto& b : node->body)
                    if (b->type == ASTType::Print)
                        stream.emit(OpCode::PRINT, {b->name});
                stream.emit(OpCode::END, {});
                break;
            default: break;
            }
        }
        stream.emit(OpCode::END, {});
        return stream;
    }
};
