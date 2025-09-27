#ifndef BESTMOVE_HH
#define BESTMOVE_HH

#include "board.hh"
#include "Move.hh"
#include "Evaluator.hh"
#include "Piece.cc"

#include <vector>
#include <limits>
#include <fstream>
//std::ofstream log("C:\\Users\\shaba\\Downloads\\engine_debug.txt");
struct move_record { //for undo
    int from;
    int to;
    char captured_piece;   // 0 if no piece captured
    bool castling_rights[4]; // previous castling rights
};
constexpr int MATE_SCORE = 32000;

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


    int negamax(board& b, int depth, int alpha, int beta, const Evaluator& eval, bool side_to_move, int half_moves){
        //int best_score = -oo;
        int best_score = -MATE_SCORE * 10;
        std::vector<Move> moves = piece.legal_moves(b,  side_to_move);
        Piece::sort_moves(moves, b,  side_to_move);

        //Mate/stalemate detection
        if(moves.empty()){
            if(piece.is_in_check(b, side_to_move)){
                //std::cout << "Mate detected on half moves " << half_moves << "\n";
                return -MATE_SCORE +half_moves;
            }
            else{
                return 0;
            }
        }


        if(depth==0){
            return eval.evaluate(b,  side_to_move);
            //return quiescence(b, alpha, beta, side_to_move, eval, half_moves);
        }

        for(Move move : moves){
            b.execute_move(move);
            int new_depth = depth-1;
            //if (piece.is_in_check(b, !side_to_move))
            //new_depth = std::min(depth, new_depth + 1);

            //log << "EXECUTE: " << move.from << "->" << move.to << ", side="
                      //<< (side_to_move ? "white" : "black") << "\n";
            //print_board_in_file(b);
            int score = -negamax(b, new_depth, -beta, -alpha, eval, !side_to_move, half_moves+1);
            //if(move.from==8) std::cout<<"Move from "<<move.from<<" to "<<move.to<<" score "<<score<< " "<<b.white_castled<<"\n";
            b.reverse_move(move);
            //log << "REVERSE: " << move.from << "->" << move.to << "\n";
            //print_board_in_file(b);


            //alpzha beta prunning
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

        //int best_score = -oo;
        int best_score = -MATE_SCORE * 10;
        Move best_move = {-1, -1};

        for (Move move : moves) {
            b.execute_move(move);
            std::cout<< "Move before rek. " << move.from << "->" << move.to  << "\n";

            //log << "Root move " << move.from << "->" << move.to << " score " << "\n";
            int score = -negamax(b, depth - 1, -beta, -alpha, eval, !side_to_move, 1);
            //log << "Root move " << move.from << "->" << move.to << " score " << score << "\n";
            std::cout<< "Move after rek. " << move.from << "->" << move.to << " score " << score << "\n";
            if (std::abs(score) > MATE_SCORE - 1000) { // heuristic: near mate range
                int mate_in = (MATE_SCORE - std::abs(score));
                if (score > 0)
                    std::cout << " (mate in " << mate_in << ")";
                else
                    std::cout << " (mated in " << mate_in << ")";
            }
            b.reverse_move(move);



            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
            /*
            alpha = std::max(alpha, best_score);
            if (alpha >= beta) break;
             */

        }

        return best_move;
    }



    int quiescence(board& b, int alpha, int beta, bool side_to_move, const Evaluator& eval, int half_moves) {
        if (piece.is_mate(b, side_to_move))
            return -100000 + half_moves;
        int static_eval = eval.evaluate(b, side_to_move);

        int best_value = static_eval;
        if(best_value >= beta) return best_value;
        if(best_value > alpha) alpha = best_value;

        std::vector<Move> noisy_moves = piece.generate_noisy_moves(b, side_to_move);


        for(auto &nm:noisy_moves){
            b.execute_move(nm);
            if (piece.is_in_check(b, side_to_move)) {
                b.reverse_move(nm);
                continue; // illegal, skip
            }
            int score = -quiescence(b, -beta, -alpha, !side_to_move, eval, half_moves+1);
            b.reverse_move(nm);

            if( score >= beta )
                return score;
            if( score > best_value )
                best_value = score;
            if( score > alpha )
                alpha = score;
        }
        return best_value;
    }

    int negamax_no_ab(board& b, int depth, const Evaluator& eval, bool side_to_move, int half_moves) {
        if (Piece::is_mate(b, side_to_move)) {
            int mate_score = -MATE_SCORE + half_moves;
            std::cout << "MATE DETECTED! side=" << (side_to_move ? "white" : "black")
                      << " score=" << mate_score << " half_moves=" << half_moves << "\n";
            return mate_score;
        }
        auto moves = piece.legal_moves(b, side_to_move);
        if (moves.empty()) {
            if (piece.is_in_check(b, side_to_move))
                return -MATE_SCORE + half_moves;
            else
                return 0;
        }
        if (depth == 0) {
            return eval.evaluate(b, side_to_move);
        }

        int best = -MATE_SCORE * 10;
        for (auto m : moves) {
            b.execute_move(m);
            if (piece.is_in_check(b, side_to_move)) { b.reverse_move(m); continue; }
            int val = -negamax_no_ab(b, depth - 1, eval, !side_to_move, half_moves + 1);
            b.reverse_move(m);
            if (val > best) best = val;
        }
        return best;
    }

    Move find_best_move_no_ab(board& b, int depth, const Evaluator& eval) {
        bool side_to_move = b.white_to_move;
        auto moves = piece.legal_moves(b, side_to_move);
        int best_score = -MATE_SCORE * 10;
        Move best_move = {-1,-1};
        for (auto m : moves) {
            b.execute_move(m);
            std::cout<< "Move before rek. " << m.from << "->" << m.to  << "\n";
            if (piece.is_in_check(b, side_to_move)) { b.reverse_move(m); continue; }
            int score = -negamax_no_ab(b, depth - 1, eval, !side_to_move, 1);
            std::cout<< "Move after rek. " << m.from << "->" << m.to << " score " << score << "\n";
            b.reverse_move(m);
            if (score > best_score) { best_score = score; best_move = m; }
        }
        return best_move;
    }






    //For debugging
    /*
    void print_board_in_file(board& b) const {
        log << "\n  +---+---+---+---+---+---+---+---+\n";

        // Печатаем доску с 8-й горизонтали (индекс 56-63) до 1-й (индекс 0-7)
        for (int rank = 7; rank >= 0; rank--) {
            log << (rank + 1) << " |";

            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                char piece = b.get_piece_at_square(square);
                log << " " << piece << " |";
            }

            log << "\n  +---+---+---+---+---+---+---+---+\n";
        }

        log << "    a   b   c   d   e   f   g   h\n\n";
    }
     */
};
#endif // BESTMOVE_HH