// ==== C++ COMPILER CORE FOR FUSION++ ====

enum class TokenType {
    IDENT, NUMBER, STRING, KEYWORD, SYMBOL, END
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    static std::vector<Token> tokenize(const std::string& input) {
        std::vector<Token> tokens;
        std::istringstream ss(input);
        std::string word;
        while (ss >> word) {
            if (word == "define" || word == "func" || word == "loop" || word == "on")
                tokens.push_back({TokenType::KEYWORD, word});
            else if (isdigit(word[0]) || (word[0] == '-' && isdigit(word[1])))
                tokens.push_back({TokenType::NUMBER, word});
            else if (word[0] == '"')
                tokens.push_back({TokenType::STRING, word});
            else if (ispunct(word[0]))
                tokens.push_back({TokenType::SYMBOL, word});
            else
                tokens.push_back({TokenType::IDENT, word});
        }
        tokens.push_back({TokenType::END, ""});
        return tokens;
    }
};

struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode> children;
};

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() const { return tokens[pos]; }
    Token advance() { return tokens[pos++]; }
    bool match(TokenType t) { return peek().type == t; }

public:
    Parser(const std::vector<Token>& t) : tokens(t) {}

    ASTNode parse() {
        ASTNode root = {"Program", "", {}};
        while (!match(TokenType::END)) {
            root.children.push_back(parseStatement());
        }
        return root;
    }

    ASTNode parseStatement() {
        if (match(TokenType::KEYWORD)) {
            std::string keyword = advance().value;
            ASTNode stmt = {"Keyword", keyword, {}};
            while (!match(TokenType::KEYWORD) && !match(TokenType::END)) {
                stmt.children.push_back({"Arg", advance().value, {}});
            }
            return stmt;
        } else {
            return {"Unknown", advance().value, {}};
        }
    }
};

class CodeGenerator {
public:
    static std::string generateCPP(const ASTNode& ast) {
        std::ostringstream cpp;
        cpp << "#include <iostream>\nint main() {\n";
        for (const auto& node : ast.children) {
            if (node.type == "Keyword" && node.value == "print") {
                for (const auto& child : node.children)
                    cpp << "    std::cout << " << child.value << " << std::endl;\n";
            } else if (node.type == "Keyword" && node.value == "loop") {
                cpp << "    for (int i = 0; i < 5; ++i) { std::cout << \"Looping\\n\"; }\n";
            }
        }
        cpp << "    return 0;\n}\n";
        return cpp.str();
    }
};

// === Test Compiler Pipeline ===
void testFusionCompilerPipeline(const std::string& source) {
    std::vector<Token> tokens = Lexer::tokenize(source);
    Parser parser(tokens);
    ASTNode ast = parser.parse();
    std::string cppCode = CodeGenerator::generateCPP(ast);
    std::ofstream out("compiled_from_fusion.cpp");
    out << cppCode;
    out.close();
    std::cout << "✅ C++ code generated: compiled_from_fusion.cpp\n";
}
