#include "UCIHandler.hh"
#include <iostream>
#include <sstream>

UCIHandler::UCIHandler( ) : tt(1 << 20) {}

void UCIHandler::loop() {
    std::string line;
    while (std::getline(std::cin, line)) {
        handleCommand(line);
    }
}

void UCIHandler::handleCommand(const std::string& cmd) {
    if (cmd == "uci") {
        std::cout << "id name BOT_KC" << std::endl;
        std::cout << "id author Shaba&Kirill" << std::endl;
        std::cout << "uciok" << std::endl;
    }
    else if (cmd == "isready") {
        std::cout << "readyok" << std::endl;
    }
    else if (cmd.rfind("position", 0) == 0) {
        //std::cout << "Before setting position, side to move = "
                  //<< (chess_board.white_to_move ? "White" : "Black") << std::endl;
        cmdPosition(cmd);
        /*
        std::cout << "After setting position, side to move = "
                  << (chess_board.white_to_move ? "White" : "Black") << std::endl;
        chess_board.print_board();
         */
    }
    else if (cmd.rfind("go", 0) == 0) {
        cmdGo(cmd);
    }
    else if (cmd == "ucinewgame") {
        chess_board.init_board();
    }
    else if (cmd == "quit") {
        exit(0);
    }
    else if(cmd == "score"){
        std::cout << eval.evaluate(chess_board, chess_board.white_to_move) << std::endl;
        chess_board.print_board();
    }
}

void UCIHandler::cmdPosition(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string token;
    iss >> token; // "position"
    iss >> token;

    if (token == "startpos") {
        chess_board.init_board();
    } else if (token == "fen") {
        std::string fen;
        std::getline(iss, fen);
        //chess_board.loadFEN(fen); //Need to do FEN
    }
    if (iss >> token && token == "moves") {
        std::string move;
        while (iss >> move) {
            int from_square = chess_board.coordToSquare(move.substr(0, 2));
            int to_square   = chess_board.coordToSquare(move.substr(2, 2));
            char promotion= '\0';
            if(move.length()==5) promotion = move[4];
            chess_board.execute_move({from_square, to_square, promotion});
        }
    }
}

void UCIHandler::cmdGo(const std::string& cmd) {
    int search_depth = 6;

    Move best = engine.find_best_move_negamax(chess_board, search_depth, eval, tt); // or whatever your best-move function is
    std::cout << "bestmove " << toUCI(best) << std::endl;
}

std::string UCIHandler::toUCI(const Move& move){
    std::string uci = chess_board.squareToCoord(move.from) + chess_board.squareToCoord(move.to);
    if (move.promotion != '\0') {
        uci += move.promotion;
    }
    return uci;
}