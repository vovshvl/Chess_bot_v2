#ifndef BESTMOVE_HH
#define BESTMOVE_HH

#include "board.hh"
#include "Move.hh"
#include "Evaluator.hh"
#include "Piece.cc"

#include <vector>
#include <limits>
struct move_record { //for undo
    int from;
    int to;
    char captured_piece;   // 0 if no piece captured
    bool castling_rights[4]; // previous castling rights
};

class Minmax{
    using Bitboard = uint64_t;
public:
    static constexpr int oo = std::numeric_limits<int>::max();

    int minmax(board& board, int depth, bool white_to_move, const Evaluator& eval, int alpha=-oo, int beta=oo){//with alphabeta prunning

        Piece piece;
        if(depth ==0)return eval.evaluate(board, white_to_move);

        int best_score = white_to_move? -oo : oo;
        auto moves = piece.legal_moves(board, white_to_move);

        for(auto[from, to]:moves) {
            char captured_piece = board.get_piece_at_square(to);
            board.execute_move(from, to);

            int score = minmax(board, depth - 1, !white_to_move, eval, alpha, beta);

            board.reverse_move(from, to, captured_piece);

            if (white_to_move) {
                if (score > best_score) best_score = score;
                if (best_score > alpha) alpha = best_score;
            } else {
                if (score < best_score) best_score = score;
                if (best_score < beta) beta = best_score;
            }
            if (alpha >= beta) break;
        }
        return best_score;
    }

    std::pair<int,int> find_best_move(board& b, int depth, bool white_to_move, const Evaluator& eval, int alpha = -oo, int beta=oo){
        auto moves = Piece::legal_moves(b, white_to_move);
        int best_score = white_to_move ? std::numeric_limits<int>::min()
                                       : std::numeric_limits<int>::max();
        std::pair<int,int> best_move = {-1,-1};

        for (auto [from, to] : moves) {
            char captured_piece = b.get_piece_at_square(to);
            b.execute_move(from, to);

            int score = minmax(b, depth - 1, !white_to_move, eval, alpha, beta);
            b.reverse_move(from, to, captured_piece);

            if (white_to_move) {
                if (score > best_score) {
                    best_score = score;
                    best_move = {from, to};
                }
                if (best_score > alpha) alpha = best_score;
            } else {
                if (score < best_score) {
                    best_score = score;
                    best_move = {from, to};
                }
                if (best_score < beta) beta = best_score;
            }
            if (alpha >= beta) break;
        }

        return best_move;
    }

    // Benchmark minmax_with_alphabeta with counter
    int minmax_benchmark(board& b, int depth, bool white_to_move,
                         const Evaluator& eval, long long& counter) {
        if (depth == 0) {
            counter++;  // count evaluated leaf node
            return eval.evaluate(b, white_to_move);
        }

        auto moves = Piece::legal_moves(b, white_to_move);
        if (moves.empty()) {
            counter++;
            return eval.evaluate(b, white_to_move);
        }

        int best_score = white_to_move ? std::numeric_limits<int>::min()
                                       : std::numeric_limits<int>::max();

        for (auto [from, to] : moves) {
            char captured_piece = b.get_piece_at_square(to);
            b.execute_move(from, to);
            int score = minmax_benchmark(b, depth - 1, !white_to_move, eval, counter);
            b.reverse_move(from, to,captured_piece);

            if (white_to_move) {
                if (score > best_score) best_score = score;
            } else {
                if (score < best_score) best_score = score;
            }
        }

        return best_score;
    }

    std::pair<int,int> find_best_move_benchmark(board& b, int depth, bool white_to_move,
                                                const Evaluator& eval, long long& counter) {
        auto moves = Piece::legal_moves(b, white_to_move);
        int best_score = white_to_move ? std::numeric_limits<int>::min()
                                       : std::numeric_limits<int>::max();
        std::pair<int,int> best_move = {-1,-1};

        for (auto [from, to] : moves) {
            char captured_piece = b.get_piece_at_square(to);
            b.execute_move(from, to);
            int score = minmax_benchmark(b, depth - 1, !white_to_move, eval, counter);
            b.reverse_move(from, to, captured_piece);

            if (white_to_move) {
                if (score > best_score) {
                    best_score = score;
                    best_move = {from, to};
                }
            } else {
                if (score < best_score) {
                    best_score = score;
                    best_move = {from, to};
                }
            }
        }

        return best_move;
    }

};
#endif // BESTMOVE_HH