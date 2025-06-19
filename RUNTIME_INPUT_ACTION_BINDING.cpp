struct InputAction {
    std::string key;
    std::function<void()> handler;
};

class InputManager {
    std::unordered_map<std::string, InputAction> bindings;

public:
    void bind(const std::string& key, std::function<void()> fn) {
        bindings[key] = {key, fn};
        std::cout << "[Input] Bound key: " << key << "\n";
    }

    void handleInput(const std::string& input) {
        if (bindings.count(input)) bindings[input].handler();
        else std::cout << "[Input] No action for key: " << input << "\n";
    }

    void listenLoop() {
        std::string in;
        std::cout << "🔄 Listening for input (type `exit` to quit):\n";
        while (true) {
            std::getline(std::cin, in);
            if (in == "exit") break;
            handleInput(in);
        }
    }
};
