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

// fusionpp_fpp_parser.h
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <iostream>

enum class NodeType { Func, Let, Print, Unknown };
struct FppNode {
    NodeType type;
    std::string name;
    std::vector<std::string> args;
    std::vector<std::shared_ptr<FppNode>> body;
    FppNode(NodeType t, const std::string& n) : type(t), name(n) {}
};

class FppParser {
public:
    std::vector<std::shared_ptr<FppNode>> parse(const std::string& file) {
        std::ifstream in(file);
        std::vector<std::shared_ptr<FppNode>> nodes;
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream ss(line);
            std::string token; ss >> token;
            if (token == "func") {
                std::string fname; ss >> fname;
                auto fn = std::make_shared<FppNode>(NodeType::Func, fname);
                while (std::getline(in, line) && line != "endfunc") {
                    std::istringstream fss(line); std::string tk; fss >> tk;
                    if (tk == "print") {
                        std::string rest; std::getline(fss, rest);
                        fn->body.push_back(std::make_shared<FppNode>(NodeType::Print, rest));
                    } else if (tk == "let") {
                        std::string n, eq, v; fss >> n >> eq >> v;
                        auto let = std::make_shared<FppNode>(NodeType::Let, n);
                        let->args = {v};
                        fn->body.push_back(let);
                    }
                }
                nodes.push_back(fn);
            }
            // Add more as needed
        }
        return nodes;
    }
};


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

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <cstdlib>

// ─────────────── Tokenizer ───────────────
struct Token {
    std::string type;
    std::string value;
};
std::vector<Token> lex(const std::string& src) {
    std::vector<Token> t;
    std::regex re(R"((\"[^\"]*\")|(\w+)|(\{|\}|\(|\)|:|;|,|=))");
    for (std::sregex_iterator i = std::sregex_iterator(src.begin(), src.end(), re), e = std::sregex_iterator(); i != e; ++i) {
        std::string v = i->str();
        std::string ty = "IDENT";
        if (v == "define") ty = "DEFINE";
        else if (v == "as") ty = "AS";
        else if (v == "derives") ty = "DERIVES";
        else if (v == "init") ty = "INIT";
        else if (v == "let") ty = "LET";
        else if (v == "instruction") ty = "INSTRUCTION";
        else if (v == "func") ty = "FUNC";
        else if (v == "on") ty = "ON";
        else if (v == "update" || v == "collision" || v == "think" || v == "ai") ty = v == "update" ? "UPDATE" : v == "collision" ? "COLLISION" : v == "think" ? "THINK" : "AI";
        else if (v == "print") ty = "PRINT";
        else if (v == "load") ty = "LOAD";
        else if (v == "stream") ty = "STREAM";
        else if (v == "tagged") ty = "TAGGED";
        else if (v == "animation") ty = "ANIMATION";
        else if (v == "asset") ty = "ASSET";
        else if (v == "behavior") ty = "BEHAVIOR";
        else if (v == "{") ty = "LBRACE";
        else if (v == "}") ty = "RBRACE";
        else if (v == "(") ty = "LPAREN";
        else if (v == ")") ty = "RPAREN";
        else if (v == ":") ty = "COLON";
        else if (v == ";") ty = "SEMICOLON";
        else if (v == "=") ty = "EQUALS";
        else if (v == ",") ty = "COMMA";
        else if (v.size() > 1 && v[0] == '"') ty = "STRING";
        t.push_back({ty, v});
    }
    return t;
}

// ─────────────── AST ───────────────
struct ASTNode {
    std::string type, value;
    std::vector<ASTNode> children;
};

size_t parse_block(const std::vector<Token>& t, size_t i, ASTNode& block);
ASTNode parse(const std::vector<Token>& t) {
    ASTNode root = {"Program", "", {}};
    size_t i = 0, n = t.size();
    while (i < n) {
        // Entity/Type Definition
        if (t[i].type == "DEFINE" && i+4 < n && t[i+2].type == "AS" && t[i+3].type == "IDENT" && t[i+4].type == "DERIVES") {
            ASTNode def = {"Definition", t[i+1].value, {
                {"BaseType", t[i+3].value, {}},
                {"Traits", t[i+5].value, {}}
            }};
            i += 6;
            root.children.push_back(def);
        }
        // Variable
        else if ((t[i].type == "INIT" || t[i].type == "LET") && i+4 < n && t[i+2].type == "EQUALS") {
            ASTNode var = {t[i].type == "INIT" ? "InitVar" : "LetVar", t[i+1].value, {
                {"Type", t[i+4].value, {}},
                {"Value", t[i+3].value, {}}
            }};
            i += 5;
            root.children.push_back(var);
        }
        // Functions/Instructions
        else if ((t[i].type == "FUNC" || t[i].type == "INSTRUCTION") && i+2 < n) {
            ASTNode fn = {t[i].type, t[i+1].value, {}};
            i += 2;
            if (t[i].type == "LPAREN") {
                ++i; // skip (
                while (t[i].type != "RPAREN") {
                    std::string argName = t[i].value;
                    std::string argType = (i+2 < n && t[i+1].type == "COLON") ? t[i+2].value : "";
                    fn.children.push_back({"Arg", argName, {{"Type", argType, {}}}});
                    i += 3;
                    if (t[i].type == "COMMA") ++i;
                }
                ++i; // skip )
            }
            if (t[i].type == "LBRACE") i = parse_block(t, i, fn);
            root.children.push_back(fn);
        }
        // Event blocks, AI logic, asset, tag, animation
        else if (t[i].type == "ON" || t[i].type == "AI" || t[i].type == "TAGGED" || t[i].type == "ANIMATION" || t[i].type == "ASSET" || t[i].type == "BEHAVIOR") {
            ASTNode block = {t[i].type, t[i+1].value, {}};
            i += 2;
            if (t[i].type == "LBRACE") i = parse_block(t, i, block);
            root.children.push_back(block);
        }
        else i++;
    }
    return root;
}
size_t parse_block(const std::vector<Token>& t, size_t i, ASTNode& block) {
    ++i; // skip {
    while (i < t.size() && t[i].type != "RBRACE") {
        if (t[i].type == "PRINT" && t[i+1].type == "STRING") {
            block.children.push_back({"Print", t[i+1].value, {}});
            i += 2;
        }
        // AI Think statement
        else if (t[i].type == "THINK" && t[i+1].type == "STRING") {
            block.children.push_back({"AIThink", t[i+1].value, {}});
            i += 2;
        }
        // Asset Load
        else if (t[i].type == "LOAD" && t[i+1].type == "STRING") {
            block.children.push_back({"AssetLoad", t[i+1].value, {}});
            i += 2;
        }
        // Animation
        else if (t[i].type == "ANIMATION" && t[i+1].type == "STRING") {
            block.children.push_back({"Animation", t[i+1].value, {}});
            i += 2;
        }
        else i++;
    }
    return i+1;
}

// ─────────────── AI Behavior Templates ───────────────
const std::map<std::string, std::string> ai_templates = {
    {"basic_enemy", R"cpp(
// --- Prebuilt AI: Basic Enemy ---
// Always move toward player, attack if close
void ai_basic_enemy(float& x, float& y, float player_x, float player_y) {
    if (player_x > x) x += 0.1f;
    else if (player_x < x) x -= 0.1f;
    if (player_y > y) y += 0.1f;
    else if (player_y < y) y -= 0.1f;
    if (abs(player_x-x) < 1.0f && abs(player_y-y) < 1.0f)
        std::cout << \"Enemy attacks player!\" << std::endl;
}
)cpp"},
    {"idle_patrol", R"cpp(
// --- Prebuilt AI: Idle Patrol ---
void ai_idle_patrol(float& x, float& y, int& dir) {
    x += (dir == 0 ? 0.05f : -0.05f);
    if (x > 10.0f) dir = 1;
    if (x < 0.0f) dir = 0;
}
)cpp"}
};

// ─────────────── CodeGen ───────────────
std::string generate_cpp(const ASTNode& ast) {
    std::ostringstream cpp;
    cpp << "#include <iostream>\n#include <cmath>\n";
    // Output AI templates used
    std::map<std::string, bool> ai_used;

    // Predeclare any needed variables
    cpp << "\nfloat player_x=0, player_y=0, enemy_x=5, enemy_y=5;\nint patrol_dir=0;\n";

    // Find if any behaviors reference templates
    for (const auto& node : ast.children) {
        if (node.type == "AI" && ai_templates.count(node.value)) {
            ai_used[node.value] = true;
        }
    }
    for (const auto& it : ai_used)
        cpp << ai_templates.at(it.first) << "\n";

    cpp << "extern \"C\" void inline_asm();\n";
    cpp << "int main() {\n";
    // Process variables, entities, instructions, event blocks, AI, etc.
    for (const auto& node : ast.children) {
        if (node.type == "InitVar" || node.type == "LetVar") {
            cpp << "    " << (node.children[0].value == "int" ? "int" : node.children[0].value)
                << " " << node.value << " = " << node.children[1].value << ";\n";
        }
        else if (node.type == "Instruction" || node.type == "FUNC") {
            cpp << "    // Instruction/Fn: " << node.value << "\n";
        }
        else if (node.type == "ON" || node.type == "UPDATE") {
            cpp << "    // Event block: " << node.value << "\n";
            for (const auto& sub : node.children)
                if (sub.type == "Print")
                    cpp << "    std::cout << " << sub.value << " << std::endl;\n";
        }
        else if (node.type == "AI") {
            if (ai_templates.count(node.value)) {
                // Demo: call AI for enemy entity
                if (node.value == "basic_enemy")
                    cpp << "    ai_basic_enemy(enemy_x, enemy_y, player_x, player_y);\n";
                else if (node.value == "idle_patrol")
                    cpp << "    ai_idle_patrol(enemy_x, enemy_y, patrol_dir);\n";
            }
            for (const auto& sub : node.children)
                if (sub.type == "AIThink")
                    cpp << "    std::cout << " << sub.value << " << std::endl;\n";
        }
        else if (node.type == "ASSET") {
            for (const auto& sub : node.children)
                if (sub.type == "AssetLoad")
                    cpp << "    std::cout << \"Loading asset: \" << " << sub.value << " << std::endl;\n";
        }
        else if (node.type == "ANIMATION") {
            for (const auto& sub : node.children)
                if (sub.type == "Animation")
                    cpp << "    std::cout << \"Play animation: \" << " << sub.value << " << std::endl;\n";
        }
    }
    cpp << "    inline_asm();\n";
    cpp << "    return 0;\n}\n";

    // Inline NASM block
    cpp << "extern \"C\" void inline_asm() {\n";
    cpp << "    asm(\"movl $0x1, %eax;\\n\\tint $0x80;\");\n";
    cpp << "}\n";
    return cpp.str();
}

// ─────────────── File IO ───────────────
std::string read_file(const std::string& f) {
    std::ifstream file(f);
    if (!file) { std::cerr << "Cannot open " << f << "\\n"; exit(1); }
    std::stringstream buf; buf << file.rdbuf(); return buf.str();
}

// ─────────────── Main ───────────────
int main(int argc, char** argv) {
    if (argc < 2) { std::cerr << "Usage: fusionpp <source.fpp>\\n"; return 1; }
    std::string src = read_file(argv[1]);
    auto tokens = lex(src);
    auto ast = parse(tokens);
    std::string cpp = generate_cpp(ast);
    std::ofstream out("fusion_game.cpp"); out << cpp; out.close();
    std::cout << "Compiling to fusion_game.exe...\\n";
    int res = system("g++ fusion_game.cpp -o fusion_game.exe");
    if (res == 0)
        std::cout << "Built: fusion_game.exe\\n";
    else
        std::cerr << "Build failed.\\n";
    return 0;
}
