#include <chrono>
#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include "../board.hh"
#include "../engine.cc"

class ChessBenchmark {
private:
    std::vector<board> test_positions;

    void generate_test_positions() {
        test_positions.clear();
        test_positions.reserve(1000);

        // Starting position
        board start_pos;
        start_pos.init_board();
        test_positions.push_back(start_pos);

        // Generate varied positions by making random moves
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> move_dist(0, 3);

        for (int pos = 0; pos < 999; pos++) {
            board b = start_pos;

            // Make 5-20 random moves to create diverse positions
            int num_moves = 5 + (pos % 16);
            for (int move = 0; move < num_moves; move++) {
                // Simple position modifications to create variety
                // (You could replace this with actual move generation)
                switch (move_dist(gen)) {
                    case 0: // Remove a pawn
                        if (pos % 3 == 0) b.white_pawn &= ~(1ULL << (8 + (pos % 8)));
                        else b.black_pawn &= ~(1ULL << (48 + (pos % 8)));
                        break;
                    case 1:
                        if (pos % 2 == 0 && b.white_knight) {
                            uint64_t knight = b.white_knight & -b.white_knight; // Get one knight
                            b.white_knight &= ~knight;
                            b.white_knight |= knight << 8; // Move forward if possible
                        }
                        break;
                    case 2:
                        if (move < 3) {
                            b.king_castle_white = (pos % 4) != 0;
                            b.queen_castle_white = (pos % 3) != 0;
                        }
                        break;
                    case 3: // Endgame position simulation
                        if (pos > 500) {
                            b.white_queen = b.black_queen = 0; // Remove queens
                            b.white_rook &= (1ULL << 7); // Keep only one rook
                            b.black_rook &= (1ULL << 56);
                        }
                        break;
                }
            }
            b.update_combined_bitboards();
            test_positions.push_back(b);
        }
    }

public:
    struct BenchmarkResult {
        double total_time;
        double evaluations_per_second;
        long long total_evaluations;
        double cache_hit_rate;
    };

    BenchmarkResult run_benchmark(int seconds_to_run = 5) {
        generate_test_positions();

        long long total_evaluations = 0;
        long long sum = 0;

        auto start_time = std::chrono::high_resolution_clock::now();
        auto end_time = start_time + std::chrono::seconds(seconds_to_run);

        while (std::chrono::high_resolution_clock::now() < end_time) {
            for (size_t i = 0; i < test_positions.size(); i++) {
                // Alternate between white and black to move
                bool white_to_move = (total_evaluations % 2) == 0;
                sum += evaluate_position(test_positions[i], white_to_move);
                total_evaluations++;


                if (total_evaluations % 15000 == 0) {
                    if (std::chrono::high_resolution_clock::now() >= end_time) {
                        break;
                    }
                }
            }
        }

        auto actual_end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = actual_end_time - start_time;

        // Prevent dead code elimination
        volatile long long prevent_optimization = sum;
        (void)prevent_optimization;

        BenchmarkResult result;
        result.total_time = elapsed.count();
        result.total_evaluations = total_evaluations;
        result.evaluations_per_second = total_evaluations / elapsed.count();

        return result;
    }

    // Stress test with multiple threads
    BenchmarkResult run_multithreaded_benchmark(int num_threads = std::thread::hardware_concurrency(), int seconds_per_thread = 3) {
        generate_test_positions();

        std::vector<std::thread> threads;
        std::vector<long long> thread_results(num_threads, 0);
        std::vector<long long> thread_sums(num_threads, 0);

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int t = 0; t < num_threads; t++) {
            threads.emplace_back([this, t, seconds_per_thread, &thread_results, &thread_sums]() {
                long long evaluations = 0;
                long long sum = 0;

                auto thread_start = std::chrono::high_resolution_clock::now();
                auto thread_end = thread_start + std::chrono::seconds(seconds_per_thread);

                while (std::chrono::high_resolution_clock::now() < thread_end) {
                    for (size_t i = 0; i < test_positions.size(); i++) {
                        bool white_to_move = ((evaluations + t) % 2) == 0;
                        sum += evaluate_position(test_positions[i], white_to_move);
                        evaluations++;

                        if (evaluations % 15000 == 0 && std::chrono::high_resolution_clock::now() >= thread_end) {
                            break;
                        }
                    }
                }

                thread_results[t] = evaluations;
                thread_sums[t] = sum;
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;

        long long total_evaluations = 0;
        long long total_sum = 0;
        for (int i = 0; i < num_threads; i++) {
            total_evaluations += thread_results[i];
            total_sum += thread_sums[i];
        }

        // Prevent optimization
        volatile long long prevent_optimization = total_sum;
        (void)prevent_optimization;

        BenchmarkResult result;
        result.total_time = elapsed.count();
        result.total_evaluations = total_evaluations;
        result.evaluations_per_second = total_evaluations / elapsed.count();
        result.cache_hit_rate = 0.0;

        return result;
    }
};

int Benchmark() {
    ChessBenchmark benchmark;

    std::cout << "=== Chess Engine Evaluation Benchmark ===\n\n";

    // Single-threaded benchmark
    std::cout << "Running single-threaded benchmark (5 seconds)...\n";
    auto single_result = benchmark.run_benchmark(5);

    std::cout << "Single-threaded Results:\n";
    std::cout << "  Time: " << single_result.total_time << " seconds\n";
    std::cout << "  Total evaluations: " << single_result.total_evaluations << "\n";
    std::cout << "  Evaluations/sec: " << static_cast<long long>(single_result.evaluations_per_second) << "\n";
    std::cout << "  Eval/sec (millions): " << (single_result.evaluations_per_second / 1000000.0) << "\n\n";

    // Multi-threaded benchmark
    int num_cores = std::thread::hardware_concurrency();
    std::cout << "Running multi-threaded benchmark (" << num_cores << " threads, 3 seconds each)...\n";
    auto multi_result = benchmark.run_multithreaded_benchmark(num_cores, 3);

    std::cout << "Multi-threaded Results:\n";
    std::cout << "  Threads: " << num_cores << "\n";
    std::cout << "  Time: " << multi_result.total_time << " seconds\n";
    std::cout << "  Total evaluations: " << multi_result.total_evaluations << "\n";
    std::cout << "  Evaluations/sec: " << static_cast<long long>(multi_result.evaluations_per_second) << "\n";
    std::cout << "  Eval/sec (millions): " << (multi_result.evaluations_per_second / 1000000.0) << "\n";
    std::cout << "  Scaling factor: " << (multi_result.evaluations_per_second / single_result.evaluations_per_second) << "x\n\n";

    // Performance classification
    if (single_result.evaluations_per_second > 10000000) {
        std::cout << "Performance: Excellent (>10M eval/sec)\n";
    } else if (single_result.evaluations_per_second > 5000000) {
        std::cout << "Performance: Good (5-10M eval/sec)\n";
    } else if (single_result.evaluations_per_second > 1000000) {
        std::cout << "Performance: Acceptable (1-5M eval/sec)\n";
    } else {
        std::cout << "Performance: Needs optimization (<1M eval/sec)\n";
    }

    return 0;
}
