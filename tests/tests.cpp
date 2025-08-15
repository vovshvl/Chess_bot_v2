//
// Created by shaba on 15.08.2025.
//
#include <gtest/gtest.h>
#include "../board.hh"
#include "../Piece.cc"
#include "../Move.hh"

TEST(BoardTest, test_check){
    board chess_board;
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
    // Black rook on e8
    chess_board.set_piece(60, 'r');
    // Black king on h8
    chess_board.set_piece(63, 'k');

    // Check detection
    bool white_in_check = Piece::is_in_check(chess_board, true);
    bool black_in_check = Piece::is_in_check(chess_board, false);

    // Assertions instead of prints
    EXPECT_TRUE(white_in_check);
    EXPECT_FALSE(black_in_check);
}

TEST(BoardTest, test_demogame){
    board chess_board;
    chess_board.init_board();

    board chess_board_before=chess_board;

    // Создаем список демонстрационных ходов
    std::vector<Move> demo_moves = {
            // Дебют - Итальянская партия
            Move(12, 28),           // 1. e2-e4
            Move(52, 36),           // 1... e7-e5
            Move(6, 21),            // 2. Ng1-f3
            Move(57, 42),           // 2... Nb8-c6
            Move(5, 26),            // 3. Bf1-c4

    };
    for (const auto& move : demo_moves){
        chess_board.execute_move(move);
    }
    //dumb test
    //Italian game test
    for(int i=0; i<64; ++i){
        if(i==12){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==5){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==6){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==57){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==52){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==28){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'P');}
        else if(i==21){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'N');}
        else if(i==26){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'B');}
        else if(i==36){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'p'); }
        else if(i==42){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'n'); }
        else{
            EXPECT_EQ(chess_board.get_piece_at_square(i),chess_board_before.get_piece_at_square(i));
        }
    }
}

TEST(BoardTest, test_move){
    board chess_board;
    chess_board.init_board();

    board chess_board_before=chess_board;

    Move move1 = Move(12, 28);
    chess_board.execute_move(move1);

    for(int i=0; i<64; ++i){
        if(i==12){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==28){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'P');}
        else{
            EXPECT_EQ(chess_board.get_piece_at_square(i),chess_board_before.get_piece_at_square(i));
        }
    }


}
