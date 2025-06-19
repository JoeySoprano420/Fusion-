// fusionpp.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>

// ───────────────────────────────────────────────────────────────
// Token
// ───────────────────────────────────────────────────────────────
struct Token {
    std::string type;
    std::string value;
};

std::vector<Token> lex(const std::string& source) {
    std::vector<Token> tokens;
    std::regex word_regex("(\\w+|\\{|\\}|\\(|\\)|:|=|;|\\\")");
    auto words_begin = std::sregex_iterator(source.begin(), source.end(), word_regex);
    auto words_end = std::sregex_iterator();
    for (auto i = words_begin; i != words_end; ++i) {
        std::string val = i->str();
        std::string type;
        if (val == "define") type = "DEFINE";
        else if (val == "as") type = "AS";
        else if (val == "derives") type = "DERIVES";
        else if (val == "on") type = "ON";
        else if (val == "update") type = "UPDATE";
        else if (val == "print") type = "PRINT";
        else if (val == "{") type = "LBRACE";
        else if (val == "}") type = "RBRACE";
        else if (val == "\"") type = "QUOTE";
        else type = "IDENT";
        tokens.push_back({type, val});
    }
    return tokens;
}

// ───────────────────────────────────────────────────────────────
// AST
// ───────────────────────────────────────────────────────────────
struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode> children;
};

ASTNode parse(const std::vector<Token>& tokens) {
    ASTNode root = {"Program", "", {}};
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& t = tokens[i];
        if (t.type == "PRINT") {
            if (i + 2 < tokens.size() && tokens[i+1].type == "QUOTE" && tokens[i+2].type == "IDENT") {
                root.children.push_back({"Print", tokens[i+2].value, {}});
                i += 3;
            }
        }
    }
    return root;
}

// ───────────────────────────────────────────────────────────────
// CodeGen
// ───────────────────────────────────────────────────────────────
std::string generate_cpp(const ASTNode& ast) {
    std::ostringstream cpp;
    cpp << "#include <iostream>\n\n";
    cpp << "int main() {\n";

    for (const auto& node : ast.children) {
        if (node.type == "Print") {
            cpp << "    std::cout << \"" << node.value << "\" << std::endl;\n";
        }
    }

    cpp << "    return 0;\n";
    cpp << "}\n";
    return cpp.str();
}

// ───────────────────────────────────────────────────────────────
// File Read
// ───────────────────────────────────────────────────────────────
std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Cannot open file: " << filename << "\n";
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ───────────────────────────────────────────────────────────────
// Main Compiler Entry
// ───────────────────────────────────────────────────────────────
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: fusionpp <source.fpp>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string source = read_file(filename);

    auto tokens = lex(source);
    auto ast = parse(tokens);
    std::string cpp_code = generate_cpp(ast);

    std::ofstream out("build_output.cpp");
    out << cpp_code;
    out.close();

    std::cout << "Generated C++ code.\n";
    std::cout << "Compiling with g++...\n";

    int result = system("g++ build_output.cpp -o game.exe");
    if (result == 0)
        std::cout << "Compilation successful. Output: game.exe\n";
    else
        std::cerr << "Compilation failed.\n";

    return 0;
}
