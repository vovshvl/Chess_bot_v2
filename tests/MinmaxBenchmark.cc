#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>
#include "../board.hh"
#include "../Evaluator.cc"
#include "../BestMove.hh" // contains Minmax and find_best_move_benchmark

class MinmaxBenchmark {
public:
    board test_board;
    Evaluator eval;

    struct BenchmarkResult {
        int depth;
        long long positions_searched;
        double time_single_move;      // seconds to find best move
        double avg_time_per_position; // seconds per position
        double positions_per_sec;     // positions evaluated per second
    };

    std::vector<BenchmarkResult> run_depth_benchmarks(Minmax& minimax, board& test_board, Evaluator& eval, const std::vector<int>& depths) {
        std::vector<BenchmarkResult> results;
        test_board.init_board();

        for (int depth : depths) {
            long long positions = 0;

            auto start = std::chrono::high_resolution_clock::now();
            minimax.find_best_move_benchmark(test_board, depth, true, eval, positions);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> elapsed = end - start;

            BenchmarkResult r;
            r.depth = depth;
            r.positions_searched = positions;
            r.time_single_move = elapsed.count();
            r.avg_time_per_position = positions > 0 ? elapsed.count() / positions : 0;
            r.positions_per_sec = positions / elapsed.count();

            results.push_back(r);
        }

        return results;
    }

    void print_results(const std::vector<BenchmarkResult>& results) {
        // Column headers
        std::cout << std::left
                  << std::setw(6)  << "Depth"
                  << std::setw(20) << "Positions Searched"
                  << std::setw(15) << "Time (s)"
                  << std::setw(20) << "Avg Time/Pos (us)"
                  << std::setw(18) << "Positions/sec"
                  << "\n";

        std::cout << "----------------------------------------------------------------------------\n";

        for (auto& r : results) {
            std::cout << std::left
                      << std::setw(6)  << r.depth
                      << std::setw(20) << r.positions_searched
                      << std::setw(15) << std::scientific << r.time_single_move
                      << std::setw(20) << std::fixed << std::setprecision(3) << r.avg_time_per_position * 1e6
                      << std::setw(18) << std::scientific << r.positions_per_sec
                      << "\n";
        }
    }

};

int minmaxBenchmark() {
    MinmaxBenchmark benchmark;

    Minmax minimax;         // local
    board test_board;
    test_board.init_board();
    Evaluator eval;

    std::vector<int> depths = {1, 2, 3, 4, 5};
    auto results = benchmark.run_depth_benchmarks(minimax, test_board, eval, depths);
    benchmark.print_results(results);

    return 0;
}
