#include <chrono>
#include <iostream>
#include "../board.hh"
#include "../engine.cc"

int main() {
    board b;

    b.init_board();

    constexpr long long iterations = 1;
    long long sum = 0;

    auto start = std::chrono::high_resolution_clock::now();
    for (long long i = 0; i < iterations; i++) {
        sum += evaluate_position(b);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " s\n";
    std::cout << "Evaluations per second: "
              << (iterations / elapsed.count()) << "\n";
    std::cout << "Ignore sum = " << sum << "\n"; // stops compiler optimizing away
}
