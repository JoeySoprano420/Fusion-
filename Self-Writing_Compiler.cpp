// ==== SELF-WRITING COMPILER EXTENSION ENGINE ====

class CompilerSelfWriter {
    CompilerAI& compilerAI;
    std::string extensionDir = "self_extensions";

public:
    CompilerSelfWriter(CompilerAI& ai) : compilerAI(ai) {
        if (!fs::exists(extensionDir))
            fs::create_directory(extensionDir);
    }

    void generateExtensionFromTopPatterns(int max = 3) {
        std::ifstream in("fusion_compiler_ai.json");
        if (!in.is_open()) return;
        json j; in >> j;

        std::vector<std::pair<std::string, float>> scores;
        for (auto& [k, v] : j.items()) {
            float score = (v["user_accepts"].get<float>() + 1.0f) / (v["user_rejects"].get<float>() + 1.0f);
            scores.push_back({k, score});
        }

        std::sort(scores.begin(), scores.end(), [](auto& a, auto& b) {
            return a.second > b.second;
        });

        int count = 0;
        for (auto& [pattern, score] : scores) {
            if (++count > max) break;

            std::string code = j[pattern]["generatedCode"];
            std::string fname = extensionDir + "/ext_" + pattern + ".fpp";
            std::ofstream out(fname);
            out << "// Auto-generated from learning pattern: " << pattern << "\n";
            out << "// Reliability: " << score << "\n\n";
            out << code << "\n";
            out.close();

            std::cout << "🧬 Wrote compiler extension: " << fname << "\n";
        }
    }

    void integrateExtensionsIntoBuild() {
        for (const auto& file : fs::directory_iterator(extensionDir)) {
            if (file.path().extension() == ".fpp") {
                std::ifstream f(file.path());
                std::stringstream buffer;
                buffer << f.rdbuf();
                std::string content = buffer.str();
                std::cout << "\n🔗 Integrating extension: " << file.path() << "\n";
                std::cout << content << "\n";
                // This content would be parsed and incorporated into the actual build pipeline
            }
        }
    }
};

// === Final Main for Self-Evolving Compiler ===
int main() {
    std::cout << "\n🧠 Fusion++ Self-Evolving Compiler Boot...\n";

    FusionCompiler fusion;
    fusion.trainFromFolder("./examples");

    std::string injected = fusion.generateCodeWithAI("ai patrol loop");
    std::cout << "\n📤 AI Injected Code:\n" << injected << "\n";

    fusion.showLearningDashboard();

    CompilerAI ai;
    CompilerSelfWriter selfWriter(ai);
    selfWriter.generateExtensionFromTopPatterns();
    selfWriter.integrateExtensionsIntoBuild();

    return 0;
}
