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
    Piece piece;
public:
    static constexpr int oo = std::numeric_limits<int>::max();

    int minmax(board& b, int depth, bool maximizing_white, const Evaluator& eval, int alpha=-oo, int beta=oo){//with alphabeta prunning
        if(depth ==0)return eval.evaluate(b, b.white_to_move);

        int best_score = -oo;
        auto moves = piece.legal_moves(b, b.white_to_move);
        Piece::sort_moves(moves, b, b.white_to_move);

        for(auto[from, to, promotion]:moves) {
            board copy_b = b;
            char captured_piece = b.get_piece_at_square(to);
            copy_b.execute_move({from, to, promotion});

            int score = -minmax(copy_b, depth - 1,!maximizing_white, eval, -alpha, -beta);
            if(score == 4000){
                std::cout<<"Move from "<<from<<" to "<<to<<" score "<<score<< " "<<copy_b.white_castled<<"\n";
            }


            copy_b.reverse_move({from, to, captured_piece});

            /*
            if (b.white_to_move==maximizing_white) {
                if (score > best_score) best_score = score;
                if (best_score > alpha) alpha = best_score;
            } else {
                if (score < best_score) best_score = score;
                if (best_score < beta) beta = best_score;
            }
             */
            if (score > best_score) best_score = score;
            if (best_score > alpha) alpha = best_score;
            if (alpha >= beta) break;
        }
        return best_score;
    }

    Move find_best_move(board& b, int depth, const Evaluator& eval, int alpha = -oo, int beta=oo){
        bool root_player_is_white = b.white_to_move;
        auto moves = Piece::legal_moves(b,b.white_to_move);
        Piece::sort_moves(moves, b, b.white_to_move);
        int best_score = -oo;
        Move best_move = {-1,-1};

        for (auto [from, to, promotion] : moves) {
           board copy_b = b;
            char captured_piece = b.get_piece_at_square(to);
            copy_b.execute_move({from, to, promotion});

            int score = -minmax(copy_b, depth - 1,!root_player_is_white, eval, -alpha, -beta);
            //std::cout<< " depth: " <<depth << " white to move: " <<b.white_to_move <<  " alpha: " <<alpha <<  " beta: " <<beta<< " score: " << score;
            std::cout<<"Move from "<<from<<" to "<<to<<" score "<<score<< " "<<copy_b.white_castled<<"\n";
            copy_b.reverse_move({from, to, captured_piece});

            /*
            if (b.white_to_move) {
                if (score > best_score) {
                    best_score = score;
                    best_move = {from, to, promotion};
                }
                if (best_score > alpha) alpha = best_score;
            } else {
                if (score < best_score) {
                    best_score = score;
                    best_move = {from, to, promotion};
                }
                if (best_score < beta) beta = best_score;
            }
             */
            if (score > best_score) {
                best_score = score;
                best_move = {from, to, promotion};
            }
            if (best_score > alpha) alpha = best_score;
            if (alpha >= beta) break;
        }

        return best_move;
    }
    // Benchmark minmax_with_alphabeta with counter
    int minmax_benchmark(board& b, int depth, bool white_to_move,
                         const Evaluator& eval, long long& counter,
                         int alpha = -oo, int beta = oo) {

        if (depth == 0) {
            counter++;  // count evaluated leaf node
            return eval.evaluate(b, white_to_move);
        }

        auto moves = Piece::legal_moves(b, white_to_move);
        Piece::sort_moves(moves, b, white_to_move);  // sort moves for better pruning

        if (moves.empty()) {
            counter++;
            return eval.evaluate(b, white_to_move);
        }

        int best_score = white_to_move ? -oo : oo;

        for (auto [from, to, promotion] : moves) {
            char captured_piece = b.get_piece_at_square(to);
            b.execute_move({from, to, promotion});

            int score = minmax_benchmark(b, depth - 1, !white_to_move, eval, counter, alpha, beta);

            b.reverse_move({from, to, captured_piece});

            if (white_to_move) {
                if (score > best_score) best_score = score;
                if (best_score > alpha) alpha = best_score;
            } else {
                if (score < best_score) best_score = score;
                if (best_score < beta) beta = best_score;
            }
            if (alpha >= beta) break;  // pruning
        }

        return best_score;
    }

    Move find_best_move_benchmark(board& b, int depth, bool white_to_move,
                                                const Evaluator& eval, long long& counter,
                                                int alpha = -oo, int beta = oo) {
        auto moves = Piece::legal_moves(b, white_to_move);
        Piece::sort_moves(moves, b, white_to_move);  // sort moves

        int best_score = white_to_move ? -oo : oo;
        Move best_move = {-1,-1};

        for (auto [from, to, promotion] : moves) {
            char captured_piece = b.get_piece_at_square(to);
            b.execute_move({from, to, promotion});

            int score = minmax_benchmark(b, depth - 1, !white_to_move, eval, counter, alpha, beta);

            b.reverse_move({from, to, captured_piece});

            if (white_to_move) {
                if (score > best_score) {
                    best_score = score;
                    best_move = {from, to, promotion};
                }
                if (best_score > alpha) alpha = best_score;
            } else {
                if (score < best_score) {
                    best_score = score;
                    best_move = {from, to, promotion};
                }
                if (best_score < beta) beta = best_score;
            }
            if (alpha >= beta) break;  // pruning
        }

        return best_move;
    }

    int negamax(board& b, int depth, int alpha, int beta, const Evaluator& eval, bool side_to_move){
        if(depth==0){
            return eval.evaluate(b,  side_to_move);
        }

        int best_score = -oo;
        std::vector<Move> moves = piece.legal_moves(b,  side_to_move);
        Piece::sort_moves(moves, b,  side_to_move);

        for(Move move : moves){
            b.execute_move(move);
            int score = -negamax(b, depth-1, -beta, -alpha, eval, !side_to_move);
            //if(move.from==8) std::cout<<"Move from "<<move.from<<" to "<<move.to<<" score "<<score<< " "<<b.white_castled<<"\n";
            b.reverse_move(move);

            if (score > best_score) best_score = score;
            alpha = std::max(alpha, score);

            if (alpha >= beta) break;
        }
        return best_score;
    }
    Move find_best_move_negamax(board& b, int depth, const Evaluator& eval, int alpha = -oo, int beta = oo) {
        bool side_to_move = b.white_to_move;
        auto moves = piece.legal_moves(b, side_to_move);
        Piece::sort_moves(moves, b, side_to_move);

        int best_score = -oo;
        Move best_move = {-1, -1};

        for (Move move : moves) {
            b.execute_move(move);
            int score = -negamax(b, depth - 1, -beta, -alpha, eval, !side_to_move);
            //std::cout<<"Move from "<<move.from<<" to "<<move.to<<" score "<<score<< " "<<b.white_castled<<"\n";
            b.reverse_move(move);

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            alpha = std::max(alpha, best_score);
            if (alpha >= beta) break;
        }

        return best_move;
    }


};
#endif // BESTMOVE_HH