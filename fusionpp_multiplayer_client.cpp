#include <iostream>
#include <string>
#include <asio.hpp> // Standalone Asio header library required (https://think-async.com/Asio/)

void send_command(const std::string& host, unsigned short port, const std::string& msg) {
    try {
        asio::io_context io;
        asio::ip::tcp::socket socket(io);
        asio::ip::tcp::resolver resolver(io);
        asio::connect(socket, resolver.resolve(host, std::to_string(port)));

        asio::write(socket, asio::buffer(msg), asio::error_code());
        std::cout << "[📡] Sent: " << msg << "\n";
    } catch (std::exception& e) {
        std::cerr << "[❌] Network error: " << e.what() << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string host = "127.0.0.1";
    unsigned short port = 8765;

    std::cout << "Fusion++ Multiplayer Client\n";
    std::cout << "Enter commands to send (type 'exit' to quit):\n";
    std::cout << "Examples: run_bytecode main  |  autorun_fpp autogen.fpp  |  reload\n\n";

    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") break;
        if (!input.empty()) {
            send_command(host, port, input);
        }
    }
    std::cout << "[👋] Client closed.\n";
    return 0;
}
