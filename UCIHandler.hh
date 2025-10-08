#include <string>
#include "BestMove.hh"
#include "board.hh"

class UCIHandler {
public:
    UCIHandler();
    void loop();  // listens for stdin commands

private:
    board chess_board;
    TranspositionTable tt;
    Evaluator eval;
    Minmax engine;
    void handleCommand(const std::string& cmd);
    void cmdPosition(const std::string& cmd);
    void cmdGo(const std::string& cmd);
    std::string toUCI(const Move& move);
};