#pragma once
#include <chrono>
#include <iostream>
#include <functional>

void timed(const std::string& label, std::function<void()> fn) {
    auto start = std::chrono::high_resolution_clock::now();
    fn();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "[Timer] " << label << " took " << elapsed.count() << " seconds.\n";
}
