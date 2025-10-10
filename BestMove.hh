#ifndef BESTMOVE_HH
#define BESTMOVE_HH

#include "board.hh"
#include "Move.hh"
#include "Evaluator.hh"
#include "Piece.cc"
#include "OpeningBook.hh"

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

class TranspositionTable {
public:
    std::vector<TTEntry> table;
    size_t table_size;

    TranspositionTable(size_t size) : table_size(size) {
        table.resize(table_size);
    }

    size_t get_index(uint64_t zobrist_key) const {
        return zobrist_key & (table_size - 1); // bitwise AND if power-of-two size
    }

    void store(const board& b, int depth, int value, uint8_t flag, const Move& best_move) {
        size_t idx = get_index(b.zobrist_key);
        table[idx] = {b.zobrist_key, depth, value, flag, best_move};
    }

    TTEntry* probe(const board& b) {
        size_t idx = get_index(b.zobrist_key);
        TTEntry& entry = table[idx];
        if (entry.best_move.from == -1) return nullptr; // empty slot
        return &entry;
    }
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

    int negamax_benchmark(board& b, int depth, int alpha, int beta, const Evaluator& eval, bool side_to_move, int half_moves, TranspositionTable& tt, long long& nodes){
        nodes++;
        if(depth==0){
            //return eval.evaluate(b,  side_to_move);
            return quiescence(b, alpha, beta, side_to_move, eval, half_moves);
        }
        //Cache try
        if (TTEntry* entry = tt.probe(b)) {
            if (entry->depth >= depth) {
                if (entry->flag == 0) return entry->score;       // exact
                if (entry->flag == 1) alpha = std::max(alpha, entry->score); // lower bound
                if (entry->flag == 2) beta  = std::min(beta, entry->score);  // upper bound
                if (alpha >= beta) return entry->score;
            }
        }
        //int best_score = -oo;
        int best_score = -MATE_SCORE * 10;
        std::vector<Move> moves = piece.legal_moves(b,  side_to_move);
        Piece::sort_moves(moves, b,  side_to_move);
        int alpha_orig = alpha;
        Move best_move_in_this_node = {-1, -1};

        //Prioritize the historicaly strongest move
        if (TTEntry* entry = tt.probe(b)) {
            if (entry->best_move.from != -1) {
                auto it = std::find_if(moves.begin(), moves.end(),
                                       [&](const Move& m){ return m == entry->best_move; });
                if (it != moves.end()) {
                    std::swap(moves[0], *it);  // move TT move to front
                }
            }
        }

        //Mate/stalemate detection
        if(moves.empty()){
            if(piece.is_in_check(b, side_to_move)){
                return -MATE_SCORE +half_moves;
            }
            else{
                return 0;
            }
        }




        for(Move move : moves){
            b.execute_move(move);
            int new_depth = depth-1;
            //if (piece.is_in_check(b, !side_to_move))
            //new_depth = std::min(depth, new_depth + 1);
            int score = -negamax_benchmark(b, new_depth, -beta, -alpha, eval, !side_to_move, half_moves+1, tt, nodes);
            //if(move.from==8) std::cout<<"Move from "<<move.from<<" to "<<move.to<<" score "<<score<< " "<<b.white_castled<<"\n";
            b.reverse_move(move);

            if (score > best_score) {
                best_score = score;
                best_move_in_this_node = move; // track the best move
            }


            //alpzha beta prunning
            if (score > best_score) best_score = score;
            alpha = std::max(alpha, score);
            if (alpha >= beta) break;
        }
        int flag;
        if (best_score <= alpha_orig) flag = 2;    // upper bound
        else if (best_score >= beta) flag = 1;     // lower bound
        else flag = 0;                             // exact

        tt.store(b, depth, best_score, flag, best_move_in_this_node);

        return best_score;
    }
    Move find_best_move_negamax_benchmark(board& b, int depth, const Evaluator& eval, TranspositionTable& tt, long long& nodes, int alpha = -oo, int beta = oo) {
        bool side_to_move = b.white_to_move;
        auto moves = piece.legal_moves(b, side_to_move);
        Piece::sort_moves(moves, b, side_to_move);

        //int best_score = -oo;
        int best_score = -MATE_SCORE * 10;
        Move best_move = {-1, -1};

        for (Move move : moves) {
            b.execute_move(move);
            int score = -negamax_benchmark(b, depth - 1, -beta, -alpha, eval, !side_to_move, 1, tt, nodes);
            //std::cout<< "Move after: "<< move.from << "->"<<move.to<< " score " << score << "\n";
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


    int negamax(board& b, int depth, int alpha, int beta, const Evaluator& eval, bool side_to_move, int half_moves, TranspositionTable& tt){
        if(depth==0){
            //return eval.evaluate(b,  side_to_move);
            return quiescence(b, alpha, beta, side_to_move, eval, half_moves);
        }
        //Cache try
        if (TTEntry* entry = tt.probe(b)) {
            if (entry->depth >= depth) {
                if (entry->flag == 0) return entry->score;       // exact
                if (entry->flag == 1) alpha = std::max(alpha, entry->score); // lower bound
                if (entry->flag == 2) beta  = std::min(beta, entry->score);  // upper bound
                if (alpha >= beta) return entry->score;
            }
        }
        //int best_score = -oo;
        int best_score = -MATE_SCORE * 10;
        std::vector<Move> moves = piece.legal_moves(b,  side_to_move);
        Piece::sort_moves(moves, b,  side_to_move);
        int alpha_orig = alpha;
        Move best_move_in_this_node = {-1, -1};

        //Prioritize the historicaly strongest move
        if (TTEntry* entry = tt.probe(b)) {
            if (entry->best_move.from != -1) {
                auto it = std::find_if(moves.begin(), moves.end(),
                                       [&](const Move& m){ return m == entry->best_move; });
                if (it != moves.end()) {
                    std::swap(moves[0], *it);  // move TT move to front
                }
            }
        }

        //Mate/stalemate detection
        if(moves.empty()){
            if(piece.is_in_check(b, side_to_move)){
                return -MATE_SCORE +half_moves;
            }
            else{
                return 0;
            }
        }

        //Move repetition detection
        if(Piece::is_threefold_repetition(b)){
            return 0;
        }




        for(Move move : moves){
            b.execute_move(move);
            int new_depth = depth-1;
            //if (piece.is_in_check(b, !side_to_move))
            //new_depth = std::min(depth, new_depth + 1);
            int score = -negamax(b, new_depth, -beta, -alpha, eval, !side_to_move, half_moves+1, tt);
            //if(move.from==8) std::cout<<"Move from "<<move.from<<" to "<<move.to<<" score "<<score<< " "<<b.white_castled<<"\n";
            b.reverse_move(move);

            if (score > best_score) {
                best_score = score;
                best_move_in_this_node = move; // track the best move
            }


            //alpzha beta prunning
            if (score > best_score) best_score = score;
            alpha = std::max(alpha, score);
            if (alpha >= beta) break;
        }
        int flag;
        if (best_score <= alpha_orig) flag = 2;    // upper bound
        else if (best_score >= beta) flag = 1;     // lower bound
        else flag = 0;                             // exact

        tt.store(b, depth, best_score, flag, best_move_in_this_node);

        return best_score;
    }
    Move find_best_move_negamax(board& b, int depth, const Evaluator& eval, TranspositionTable& tt, int alpha = -oo, int beta = oo) {
        bool side_to_move = b.white_to_move;


        if(b.is_opening){
            Move book_move = OpeningBook::getMove(b);
            if (book_move.from != -1) {
                return book_move; // play the book move immediately
            }
        }



        auto moves = piece.legal_moves(b, side_to_move);
        Piece::sort_moves(moves, b, side_to_move);

        //int best_score = -oo;
        int best_score = -MATE_SCORE * 10;
        Move best_move = {-1, -1};

        for (Move move : moves) {
            b.execute_move(move);
            int score = -negamax(b, depth - 1, -beta, -alpha, eval, !side_to_move, 1, tt);
            //std::cout<< "Move after: "<< move.from << "->"<<move.to<< " score " << score << "\n";
            b.reverse_move(move);
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }

            alpha = std::max(alpha, best_score);
            if (alpha >= beta) break;
        }

        if(best_move.from == -1 && best_move.to == -1){
            std::cout << "End of the game" << std::endl;
            std::exit(0);
        }
        return best_move;
    }



    int quiescence(board& b, int alpha, int beta, bool side_to_move, const Evaluator& eval, int half_moves) {
        int static_eval = eval.evaluate(b, side_to_move);

        int best_value = static_eval;
        if(best_value >= beta) return best_value;
        if(best_value > alpha) alpha = best_value;

        if (static_eval + 200 < alpha)
            return alpha;

        if (half_moves > 6) return static_eval;

        std::vector<Move> noisy_moves = piece.generate_noisy_moves(b, side_to_move);


        for(auto &nm:noisy_moves){
            b.execute_move(nm);
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