#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>
#include "../board.hh"
#include "../Evaluator.hh"
#include "../BestMove.hh" // contains Minmax and find_best_move_benchmark

class Game{
public:
    void engine_vs_engine(){
        board b;
        b.init_board();
        int move_count=0;
        Minmax minimax;
        Evaluator eval;

        size_t tt_size = 1 << 20; // 1M entries
        TranspositionTable tt(tt_size);

        b.print_board();

        while(move_count<100){
            //b.white_to_move = true;
            Move white_move = minimax.find_best_move_negamax(b, 6, eval, tt);
            b.execute_move({white_move.from, white_move.to, white_move.promotion});
            move_count++;
            std::cout << "Move: " << b.squareToCoord(white_move.from) << "->" << b.squareToCoord(white_move.to) << "\n";
            b.print_board();
            //b.white_to_move = false;
            Move black_move = minimax.find_best_move_negamax(b, 6, eval, tt);
            b.execute_move({black_move.from, black_move.to, black_move.promotion});
            move_count++;
            std::cout << "Move: " << b.squareToCoord(black_move.from) << "->" << b.squareToCoord(black_move.to) << "\n";
            b.print_board();
        }
    }
};

int game(){
    Game g;
    g.engine_vs_engine();
    return 0;
}