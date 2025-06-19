#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <cstdlib>

// === Token Types ===
enum class TokenType {
    DEFINE, DERIVE, IDENT, NUMBER, STRING,
    INSTRUCTION, ON, LOAD, STREAM, ASM, INIT, LET,
    TAGGED, WRAP, EMIT, PATCH,
    LBRACE, RBRACE, LPAREN, RPAREN, COLON, SEMICOLON, EQUAL,
    UNKNOWN, END
};

struct Token {
    TokenType type;
    std::string value;
};

// === Lexer ===
std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    std::regex word_regex("(\\w+|\\\"[^\"]*\\\"|\\{|\\}|\\(|\\)|:|;|=)");
    auto begin = std::sregex_iterator(source.begin(), source.end(), word_regex);
    auto end = std::sregex_iterator();
    for (auto i = begin; i != end; ++i) {
        std::string val = i->str();
        TokenType type = TokenType::UNKNOWN;
        if (val == "define") type = TokenType::DEFINE;
        else if (val == "derive") type = TokenType::DERIVE;
        else if (val == "instruction") type = TokenType::INSTRUCTION;
        else if (val == "on") type = TokenType::ON;
        else if (val == "load") type = TokenType::LOAD;
        else if (val == "stream") type = TokenType::STREAM;
        else if (val == "asm") type = TokenType::ASM;
        else if (val == "init") type = TokenType::INIT;
        else if (val == "let") type = TokenType::LET;
        else if (val == "tagged") type = TokenType::TAGGED;
        else if (val == "wrap") type = TokenType::WRAP;
        else if (val == "emit") type = TokenType::EMIT;
        else if (val == "patch") type = TokenType::PATCH;
        else if (val == "{") type = TokenType::LBRACE;
        else if (val == "}") type = TokenType::RBRACE;
        else if (val == "(") type = TokenType::LPAREN;
        else if (val == ")") type = TokenType::RPAREN;
        else if (val == ":") type = TokenType::COLON;
        else if (val == ";") type = TokenType::SEMICOLON;
        else if (val == "=") type = TokenType::EQUAL;
        else if (val[0] == '"') type = TokenType::STRING;
        else if (isdigit(val[0]) || val[0] == '-') type = TokenType::NUMBER;
        else type = TokenType::IDENT;
        tokens.push_back({type, val});
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}

// === AST ===
struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode> children;
};

// === Parser ===
class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() const { return tokens[pos]; }
    Token advance() { return tokens[pos++]; }

public:
    Parser(const std::vector<Token>& toks) : tokens(toks) {}

    ASTNode parse() {
        ASTNode program = {"Program", "", {}};
        while (peek().type != TokenType::END) {
            program.children.push_back(parseStatement());
        }
        return program;
    }

    ASTNode parseStatement() {
        Token t = advance();
        ASTNode node;

        switch (t.type) {
            case TokenType::DEFINE:
                node = {"Define", "", {}};
                node.children.push_back({"Name", advance().value, {}});
                if (peek().type == TokenType::DERIVE) {
                    advance();
                    node.children.push_back({"Derive", advance().value, {}});
                }
                break;
            case TokenType::INSTRUCTION:
                node = {"Instruction", advance().value, {}};
                break;
            case TokenType::ON:
                node = {"On", advance().value, {}};
                break;
            case TokenType::INIT:
            case TokenType::LET: {
                node = {t.type == TokenType::INIT ? "Init" : "Let", "", {}};
                std::string name = advance().value;
                advance(); // '='
                std::string value = advance().value;
                node.children.push_back({"Var", name, {}});
                node.children.push_back({"Value", value, {}});
                break;
            }
            case TokenType::ASM:
                node = {"ASM", "inline", {}};
                break;
            default:
                node = {"Unknown", t.value, {}};
                break;
        }
        return node;
    }
};

// === Code Generator ===
std::string generateCPP(const ASTNode& ast) {
    std::ostringstream out;
    out << "#include <iostream>\n\nint main() {\n";

    for (const auto& node : ast.children) {
        if (node.type == "Define") {
            out << "    struct " << node.children[0].value << " {\n";
            out << "        float x = 0, y = 0;\n";
            out << "        void draw() { std::cout << \"Draw at \" << x << \",\" << y << std::endl; }\n";
            out << "    };\n";
        } else if (node.type == "Init" || node.type == "Let") {
            out << "    auto " << node.children[0].value << " = " << node.children[1].value << ";\n";
        } else if (node.type == "On") {
            out << "    std::cout << \"Event: " << node.value << "\" << std::endl;\n";
        } else if (node.type == "ASM") {
            out << "    __asm__(\"movl $0x1, %eax; int $0x80;\");\n";
        }
    }

    out << "    return 0;\n}\n";
    return out.str();
}

// === Main ===
int main() {
    std::ifstream file("program.fpp");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    auto tokens = tokenize(source);
    Parser parser(tokens);
    ASTNode ast = parser.parse();
    std::string cppCode = generateCPP(ast);

    std::ofstream out("program.cpp");
    out << cppCode;
    out.close();

    std::system("g++ program.cpp -o program.exe");
    std::cout << "✅ FUSION++ compiled to C++ and built as executable.\n";

    return 0;
}
