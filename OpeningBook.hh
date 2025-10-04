#ifndef OPENINGBOOK_HH
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <ctime>
#include "board.hh"

#define OPENINGBOOK_HH
class OpeningBook {
public:
    OpeningBook() {
        srand(time(0)); // random seed


        book[""] = {"e2e4", "d2d4"};  // first move options
        book["e2e4"] = {"e7e5"}; // after 1.e4
        book["d2d4"] = {"d7d5", "g8f6"}; // after 1.d4
        book["e2e4e7e5"] = {"g1f3", "f1c4"}; // after 1.e4 e5
    }

    // Get a move from the opening book based on current moves
    static Move getMove(board& chess_board) {
        std::string movesSoFar;
        std::vector<Move> history_moves = chess_board.history_moves;
        for(Move move : history_moves){
            std::string from_string = chess_board.squareToCoord(move.from);
            std::string to_string = chess_board.squareToCoord(move.to);
            movesSoFar.append(from_string+to_string);
        }
        auto it = book.find(movesSoFar);
        if (it != book.end()) {
            const std::vector<std::string>& options = it->second;
            int index = rand() % options.size();
            std::string chosen = options[0];

            int from_square = chess_board.coordToSquare(chosen.substr(0, 2));
            int to_square   = chess_board.coordToSquare(chosen.substr(2, 2));

            return {from_square, to_square};
        }
        return {-1, -1}; // not in book
    }

private:
    static std::map<std::string, std::vector<std::string>> book;
};
#endif