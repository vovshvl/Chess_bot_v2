#include <iostream>
#include <string>
#include <chrono>
#include "ChessGameDemo.hh"


int main() {
    ChessGameDemo game;
    game.test_check();
    game.play_demo_game();
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    

    game.play_endgame_demo();

    char choice;
    std::cout << "\nDo u want to try ineractive mode (y/n): ";
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        ChessGameDemo interactive_game;
        interactive_game.interactive_mode();
    }
    
    return 0;
}