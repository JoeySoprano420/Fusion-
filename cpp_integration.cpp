#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

void runGPTGen(const std::string& prompt) {
    std::string cmd = "python3 gpt_function_client.py \"" + prompt + "\" > tmp_output.fpp";
    std::cout << "[GPT] Generating function for: " << prompt << "\n";
    system(cmd.c_str());

    std::ifstream in("tmp_output.fpp");
    std::ofstream out("autogen.fpp", std::ios::app);
    out << "\n";
    std::string line;
    while (std::getline(in, line)) {
        out << line << "\n";
    }

    std::cout << "[Injected] Code written to autogen.fpp\n";
}
