#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

enum class ASTType {
    Import, Function, Event, Entity, Let, Define, Print
};

struct ASTNode {
    ASTType type;
    std::string name;
    std::vector<std::string> args;
    std::vector<std::shared_ptr<ASTNode>> body;

    ASTNode(ASTType t, const std::string& n) : type(t), name(n) {}
};
