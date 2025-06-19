#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <iostream>

enum class ASTType { Import, Function, Event, Entity, Let, Define, Print };

struct ASTNode {
    ASTType type;
    std::string name;
    std::vector<std::string> args;
    std::vector<std::shared_ptr<ASTNode>> body;
    ASTNode(ASTType t, const std::string& n) : type(t), name(n) {}
};

enum class OpCode { LOAD_CONST, PRINT, CALL, END };

struct Bytecode {
    OpCode op;
    std::vector<std::string> args;
};

class BytecodeStream {
public:
    std::vector<Bytecode> code;
    void emit(OpCode op, std::vector<std::string> args = {}) {
        code.push_back({op, args});
    }
};
