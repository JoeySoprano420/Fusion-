#include "auto_function_builder.h"

int main() {
    FPPFunctionBuilder builder;

    std::string userIntent;
    std::cout << "🧠 Enter your function intent (e.g., 'start mission'):\n> ";
    std::getline(std::cin, userIntent);

    std::string generatedCode = builder.generateFunction(userIntent);
    std::cout << "👇 Generated Function:\n" << generatedCode;

    std::string targetFile = "autogen.fpp";
    builder.injectFunctionToFile(targetFile, generatedCode);

    return 0;
}
