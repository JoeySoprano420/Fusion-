#pragma once
#include "fpp_ast.h"
#include <fstream>
#include <sstream>
#include <map>

class FPPParser {
public:
    std::vector<std::shared_ptr<ASTNode>> parse(const std::string& filepath) {
        std::ifstream in(filepath);
        std::vector<std::shared_ptr<ASTNode>> nodes;
        std::string line;

        while (std::getline(in, line)) {
            std::istringstream ss(line);
            std::string keyword;
            ss >> keyword;

            if (keyword == "import") {
                std::string path;
                ss >> path;
                auto node = std::make_shared<ASTNode>(ASTType::Import, path.substr(1, path.length() - 2));
                nodes.push_back(node);
                auto sub = parse(node->name);
                nodes.insert(nodes.end(), sub.begin(), sub.end());
            }
            else if (keyword == "func") {
                std::string name;
                ss >> name;
                auto fn = std::make_shared<ASTNode>(ASTType::Function, name);
                while (std::getline(in, line) && line != "end") {
                    auto print = std::make_shared<ASTNode>(ASTType::Print, line);
                    fn->body.push_back(print);
                }
                nodes.push_back(fn);
            }
            else if (keyword == "print") {
                std::string rest;
                std::getline(ss, rest);
                nodes.push_back(std::make_shared<ASTNode>(ASTType::Print, rest));
            }
            else if (keyword == "let" || keyword == "init") {
                std::string name, eq, value;
                ss >> name >> eq >> value;
                auto node = std::make_shared<ASTNode>(ASTType::Let, name);
                node->args = { value };
                nodes.push_back(node);
            }
            else if (keyword == "define") {
                std::string t, n;
                ss >> t >> n;
                auto node = std::make_shared<ASTNode>(ASTType::Define, n);
                node->args = { t };
                nodes.push_back(node);
            }
        }
        return nodes;
    }
};
