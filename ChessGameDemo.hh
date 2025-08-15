//
// Created by Kirill on 8/10/2025.
//
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include "board.hh"
#include "Move.hh"
#include "Piece.cc"
#ifndef CHESS_BOT_NEW_CHESSGAMEDEMO_HH
#define CHESS_BOT_NEW_CHESSGAMEDEMO_HH

class ChessGameDemo {
private:
    board chess_board;
    std::vector<Move> move_history;
    bool white_to_move = true;
    int move_number = 1;

    // Конвертация координат в шахматную нотацию
    std::string square_to_notation(int square) const {
        int file = square % 8;
        int rank = square / 8;
        char file_char = 'a' + file;
        char rank_char = '1' + rank;
        return std::string(1, file_char) + std::string(1, rank_char);
    }

    // Получение названия фигуры
    std::string get_piece_name(char piece) const {
        switch (piece) {
            case 'P': case 'p': return "pawn";
            case 'N': case 'n': return "knight";
            case 'B': case 'b': return "bishop";
            case 'R': case 'r': return "rook";
            case 'Q': case 'q': return "queen";
            case 'K': case 'k': return "king";
            default: return "unrecognised piece";
        }
    }

    // Выполнить ход на доске
    void execute_move(const Move& move) {
        int from = move.get_from_square();
        int to = move.get_to_square();

        char moving_piece = chess_board.get_piece_at_square(from);
        char captured_piece = chess_board.get_piece_at_square(to);

        // Перемещаем фигуру
        chess_board.clear_square(from);
        chess_board.set_piece(to, moving_piece);

        // Добавляем ход в историю
        move_history.push_back(move);

        // Выводим информацию о ходе
        print_move_info(move, moving_piece, captured_piece);
    }

    void print_move_info(const Move& move, char moving_piece, char captured_piece) {
        std::string color = white_to_move ? "White" : "Black";
        std::string from_notation = square_to_notation(move.get_from_square());
        std::string to_notation = square_to_notation(move.get_to_square());
        std::string piece_name = get_piece_name(moving_piece);

        std::cout << "\n" << move_number << ". " << color << ": ";
        std::cout << piece_name << " " << from_notation << " -> " << to_notation;

        if (captured_piece != '.') {
            std::cout << " (takes: " << get_piece_name(captured_piece) << ")";
        }

        std::cout << std::endl;

        if (!white_to_move) {
            move_number++;
        }
        white_to_move = !white_to_move;
    }

    void pause_for_effect() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

public:
    ChessGameDemo() {
        chess_board.init_board();
    }

    void play_demo_game() {
        std::cout << "demo" << std::endl;
        std::cout << "Start position" << std::endl;
        chess_board.print_board();
        pause_for_effect();

        // Создаем список демонстрационных ходов
        std::vector<Move> demo_moves = {
            // Дебют - Итальянская партия
            Move(12, 28),           // 1. e2-e4
            Move(52, 36),           // 1... e7-e5
            Move(6, 21),            // 2. Ng1-f3
            Move(57, 42),           // 2... Nb8-c6
            Move(5, 26),            // 3. Bf1-c4

        };

        // Выполняем ходы
        for (const auto& move : demo_moves) {
            execute_move(move);
            chess_board.print_board();
            pause_for_effect();
        }

        print_game_summary();
    }

    void play_endgame_demo() {
        std::cout << "\nEndgame demo" << std::endl;

        // Создаем эндшпильную позицию
        chess_board.reset_board();

        // Очищаем доску и расставляем только несколько фигур
        chess_board.white_pawn = chess_board.black_pawn = 0;
        chess_board.white_knight = chess_board.black_knight = 0;
        chess_board.white_bishop = chess_board.black_bishop = 0;
        chess_board.white_rook = chess_board.black_rook = 0;
        chess_board.white_queen = chess_board.black_queen = 0;
        chess_board.white_king = chess_board.black_king = 0;

        // Король + ферзь против короля
        chess_board.set_piece(4, 'K');     // Белый король на e1
        chess_board.set_piece(3, 'Q');     // Белый ферзь на d1
        chess_board.set_piece(60, 'k');    // Черный король на e8
        chess_board.set_piece(16, 'P');    // Белая пешка на a3
        chess_board.set_piece(48, 'p');    // Черная пешка на a7

        white_to_move = true;
        move_number = 1;

        std::cout << "Endgame position:" << std::endl;
        chess_board.print_board();
        pause_for_effect();

        std::vector<Move> endgame_moves = {
            Move(3, 35),            // 1. Qd1-d5
            Move(60, 61),           // 1... Ke8-f8
            Move(35, 43),           // 2. Qd5-d6
            Move(61, 62),           // 2... Kf8-g8
            Move(43, 51),           // 3. Qd6-d7
            Move(62, 61),           // 3... Kg8-f8
            Move(4, 5),             // 4. Ke1-f2
            Move(48, 40),           // 4... a7-a5
            Move(51, 59),           // 5. Qd7-d8# (мат!)
        };

        for (const auto& move : endgame_moves) {
            execute_move(move);
            chess_board.print_board();

            if (&move == &endgame_moves.back()) {
                std::cout << "\n White won" << std::endl;
            }

            pause_for_effect();
        }
    }

    void print_game_summary() {
        std::cout << "\nGame Result" << std::endl;
        std::cout << "total moves " << move_history.size() << std::endl;
        std::cout << "History of game:" << std::endl;

        for (size_t i = 0; i < move_history.size(); i++) {
            const Move& move = move_history[i];
            std::string color = (i % 2 == 0) ? "White" : "Black";
            std::string from = square_to_notation(move.get_from_square());
            std::string to = square_to_notation(move.get_to_square());

            if (i % 2 == 0) {
                std::cout << (i / 2 + 1) << ". ";
            }
            std::cout << from << "-" << to;

            if (i % 2 == 1) {
                std::cout << std::endl;
            } else {
                std::cout << " ";
            }
        }

        if (move_history.size() % 2 == 1) {
            std::cout << std::endl;
        }

        std::cout << "\ngame finished" << std::endl;
    }

    void interactive_mode() {
        std::cout << "\ninteractive mode" << std::endl;
        std::cout << "Move format e2 e4" << std::endl;
        chess_board.print_board();

        std::string from_str, to_str;
        while (true) {
            std::cout << "\nGive move or quit to exit";
            std::cin >> from_str;

            if (from_str == "quit") break;

            std::cin >> to_str;

            // Простое преобразование нотации в индексы
            if (from_str.length() == 2 && to_str.length() == 2) {
                int from_square = (from_str[0] - 'a') + (from_str[1] - '1') * 8;
                int to_square = (to_str[0] - 'a') + (to_str[1] - '1') * 8;

                if (from_square >= 0 && from_square < 64 && to_square >= 0 && to_square < 64) {
                    Move user_move(from_square, to_square);
                    execute_move(user_move);
                    chess_board.print_board();
                } else {
                    std::cout << "incorrect square coordinates";
                }
            } else {
                std::cout << "Incorrect square format" << std::endl;
            }
        }
    }

    void test_check(){
        chess_board.reset_board();

        // Clear the board completely
        chess_board.white_pawn = chess_board.black_pawn = 0;
        chess_board.white_knight = chess_board.black_knight = 0;
        chess_board.white_bishop = chess_board.black_bishop = 0;
        chess_board.white_rook = chess_board.black_rook = 0;
        chess_board.white_queen = chess_board.black_queen = 0;
        chess_board.white_king = chess_board.black_king = 0;

        // White king on e1
        chess_board.set_piece(4, 'K');
        // Black rook on e8 (this puts white in check)
        chess_board.set_piece(60, 'r');
        // Black king somewhere safe, say h8
        chess_board.set_piece(63, 'k');

        // Show the board
        std::cout << "Test position:" << std::endl;
        chess_board.print_board();

        // Check detection
        bool white_in_check = Piece::is_in_check(chess_board, true);
        bool black_in_check = Piece::is_in_check(chess_board, false);

        std::cout << "White in check? " << (white_in_check ? "Yes" : "No") << std::endl;
        std::cout << "Black in check? " << (black_in_check ? "Yes" : "No") << std::endl;
    }
};

#endif //CHESS_BOT_NEW_CHESSGAMEDEMO_HH