// pseudo-snippet in C++
bool useLoRA = false;

void toggle_ai_mode(std::string mode) {
    if (mode == "gpt") useLoRA = false;
    else if (mode == "lora") useLoRA = true;
}

std::string get_ai_completion(const std::string& prompt) {
    if (useLoRA)
        return LoRA_Complete(prompt);  // your local model logic
    else
        return GPT4_Complete(prompt);  // via OpenAI API
}
