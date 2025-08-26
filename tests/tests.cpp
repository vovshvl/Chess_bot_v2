//
// Created by shaba on 15.08.2025.
//
#include <gtest/gtest.h>
#include "../board.hh"
#include "../Piece.cc"
#include "../Move.hh"

using Bitboard = uint64_t;

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

//pawn_move test
TEST(PieceTest, test_pawn_move){
    board chess_board;
    chess_board.init_board();

    chess_board.set_piece(45, 'N'); //to check attack capture for black pawn on e5
    chess_board.set_piece(43, 'b');//to check if pawn can be stopped by own pieces

    int white_pawn_sq = 12;
    Bitboard white_result_moves = 0;
    white_result_moves |= (1ULL <<20);//e3
    white_result_moves |= (1ULL <<28);//e4

    int black_pawn_sq_1 = 52;
    Bitboard black_result_moves_1 = 0;
    black_result_moves_1 |= (1ULL <<44);//e6
    black_result_moves_1 |= (1ULL <<36);//e5
    black_result_moves_1 |= (1ULL <<45);//f4

    int black_pawn_sq_2 = 51;
    Bitboard black_result_moves_2 = 0;


    Bitboard white_moves = Piece::pawn_moves(white_pawn_sq, true, chess_board);
    Bitboard black_moves_1 = Piece::pawn_moves(black_pawn_sq_1, false, chess_board);
    Bitboard black_moves_2 = Piece::pawn_moves(black_pawn_sq_2, false, chess_board);
    EXPECT_EQ(white_moves, white_result_moves);
    EXPECT_EQ(black_moves_1, black_result_moves_1);
    EXPECT_EQ(black_moves_2, black_result_moves_2);
}

//knight_move test
TEST(PieceTest, test_knight_move){
    board chess_board;
    chess_board.reset_board();

    // Clear the board completely
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White knight on d3
    chess_board.set_piece(19, 'N');
    // Black knight on d6
    chess_board.set_piece(40, 'n');
    // White bishop on f4
    chess_board.set_piece(29, 'B');
    // Black rook on e1
    chess_board.set_piece(4, 'r');


    int white_knight_sq = 19;
    Bitboard white_result_moves = 0;
    white_result_moves |= (1ULL <<2);//c1
    white_result_moves |= (1ULL <<4);//e1
    white_result_moves |= (1ULL <<9);//b2
    white_result_moves |= (1ULL <<13);//f2
    white_result_moves |= (1ULL <<25);//b4
    white_result_moves |= (1ULL <<34);//c5
    white_result_moves |= (1ULL <<36);//e5

    int black_knight_sq = 40;
    Bitboard black_result_moves = 0;
    black_result_moves |= (1ULL <<57);//b8
    black_result_moves |= (1ULL <<50);//c7
    black_result_moves |= (1ULL <<34);//c5
    black_result_moves |= (1ULL <<25);//b4


    Bitboard white_moves = Piece::knight_moves(white_knight_sq, true, chess_board);
    EXPECT_EQ(white_moves, white_result_moves);

    Bitboard black_moves = Piece::knight_moves(black_knight_sq, false, chess_board);
    EXPECT_EQ(black_moves, black_result_moves);
}
// rook_move test
TEST(PieceTest, test_rook_move) {
    board chess_board;
    chess_board.reset_board();

    // Clear the board completely
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // Place white rook on d4 (square 27)
    chess_board.set_piece(27, 'R');

    // Place blockers
    chess_board.set_piece(19, 'P'); // white pawn on d3 (blocks down)
    chess_board.set_piece(35, 'p'); // black pawn on d5 (can capture up)
    chess_board.set_piece(29, 'B'); // white bishop on f4 (blocks right)
    chess_board.set_piece(24, 'n'); // black knight on a4 (can capture left)

    int white_rook_sq = 27;
    Bitboard white_result_moves = 0;
    white_result_moves |= (1ULL <<24);//a4
    white_result_moves |= (1ULL <<25);//b4
    white_result_moves |= (1ULL <<26);//c4
    white_result_moves |= (1ULL <<28);//e4
    white_result_moves |= (1ULL <<35);//d5


    // Run function
    Bitboard white_rook_moves = Piece::rook_moves(27, true, chess_board);

    EXPECT_EQ(white_rook_moves, white_result_moves);
}
TEST(PieceTest, test_bishop_move) {
    board chess_board;
    chess_board.reset_board();
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White bishop on d4 (square 27)
    chess_board.set_piece(27, 'B');
    // Black bishop on f6 (square 45)
    chess_board.set_piece(45, 'b');
    // White pawn on c3 (square 18) - blocks white bishop SW
    chess_board.set_piece(18, 'P');
    // Black rook on e5 (square 36) - can be captured by white bishop
    chess_board.set_piece(36, 'r');
    // White knight on g7 (square 54) - can be captured by black bishop
    chess_board.set_piece(54, 'N');
    // Black pawn on e7 (square 52) - blocks black bishop NE
    chess_board.set_piece(52, 'p');

    // Expected moves for white bishop on d4
    Bitboard white_result_moves = 0;
    white_result_moves |= (1ULL << 36); // e5 (capture)
    white_result_moves |= (1ULL << 34); // c5
    white_result_moves |= (1ULL << 41); // b6
    white_result_moves |= (1ULL << 48); // a7
    white_result_moves |= (1ULL << 20); // e3
    white_result_moves |= (1ULL << 13); // f2
    white_result_moves |= (1ULL << 6);  // g1


    Bitboard black_result_moves = 0;
    black_result_moves |= (1ULL << 54); // g7 (capture)
    black_result_moves |= (1ULL << 38); // g5
    black_result_moves |= (1ULL << 31); // h4


    Bitboard white_moves = Piece::bishop_attacks(27,true,  chess_board);
    Bitboard black_moves = Piece::bishop_attacks(45,false, chess_board);

    EXPECT_EQ(white_moves, white_result_moves);
    EXPECT_EQ(black_moves, black_result_moves);
}

TEST(PieceTest, test_attackers){
    board chess_board;
    chess_board.reset_board();
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White pawn on d4(27)
    chess_board.set_piece(27, 'P');
    // White rook on d7(51)
    chess_board.set_piece(51, 'R');
    // Black rook on d8(59)
    chess_board.set_piece(59, 'r');
    // Black rook on d1(3)
    chess_board.set_piece(3, 'r');
    // Black rook on c1(2)
    chess_board.set_piece(2, 'r');
    // Black knight on h1(7)
    chess_board.set_piece(7, 'n');
    // Black knight on f5(37)
    chess_board.set_piece(37, 'n');
    // Black queen on a4(24)
    chess_board.set_piece(24, 'q');
    // Black queen on h6(47)
    chess_board.set_piece(47, 'q');
    // Black pawn on e5(36)
    chess_board.set_piece(36, 'p');

    std::vector<std::pair<int, char>> white_pawn_result_attackers;
    white_pawn_result_attackers.emplace_back(3,'r');
    white_pawn_result_attackers.emplace_back(24,'q');
    white_pawn_result_attackers.emplace_back(36,'p');
    white_pawn_result_attackers.emplace_back(37,'n');

    std::vector<std::pair<int, char>> white_attackers = Piece::attackers(27, true, chess_board);
    std::sort(white_attackers.begin(), white_attackers.end());

    EXPECT_EQ(white_pawn_result_attackers, white_attackers);

}
TEST(PieceTest, test_legal_moves_single_check_without_block) {
    board chess_board;
    chess_board.reset_board();

    // Clear the board completely
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White king on e1 (square 4)
    chess_board.set_piece(4, 'K');
    // White rook on d1 (square 3)
    chess_board.set_piece(3, 'R');
    // Black rook giving check on e8 (square 60)
    chess_board.set_piece(60, 'r');
    // Black king on h8 (square 63)
    chess_board.set_piece(63, 'k');

    std::vector<std::pair<int,int>> white_legal_moves = Piece::legal_moves(chess_board, true);

    std::vector<std::pair<int,int>> expected_moves = {
            {4, 5}, {4, 11}, {4, 13},
    };

    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);
}

TEST(PieceTest, test_legal_moves_single_check_with_block) {
    board chess_board;
    chess_board.reset_board();

    // Clear the board completely
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White king on e1 (square 4)
    chess_board.set_piece(4, 'K');
    // White rook on d2 (square 11)
    chess_board.set_piece(11, 'R');
    // Black rook giving check on e8 (square 60)
    chess_board.set_piece(60, 'r');
    // Black king on h8 (square 63)
    chess_board.set_piece(63, 'k');

    std::vector<std::pair<int,int>> white_legal_moves = Piece::legal_moves(chess_board, true);

    std::vector<std::pair<int,int>> expected_moves = {
            {4, 3}, {4, 5}, {4, 13}, {11, 12}
    };

    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);
}

TEST(PieceTest, test_legal_moves_double_check_with_block) {
    board chess_board;
    chess_board.reset_board();

    // Clear the board completely
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White king on e1 (square 4)
    chess_board.set_piece(4, 'K');
    // White rook on d2 (square 11)
    chess_board.set_piece(11, 'R');
    // Black rook giving check on e8 (square 60)
    chess_board.set_piece(60, 'r');
    // Black bishop on h4 (square 31)
    chess_board.set_piece(31, 'b');
    // Black king on h8 (square 63)
    chess_board.set_piece(63, 'k');

    std::vector<std::pair<int,int>> white_legal_moves = Piece::legal_moves(chess_board, true);

    std::vector<std::pair<int,int>> expected_moves = {
            {4, 3}, {4, 5}
    };

    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);
}

TEST(BoardTest, test_mate) {
    board chess_board;
    chess_board.reset_board();

    // Clear the board completely
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // White king on a1a (square 0)
    chess_board.set_piece(0, 'K');
    // Black rook on h1 (square 7)
    chess_board.set_piece(7, 'r');
    // Black rook on g2 (square 7)
    chess_board.set_piece(14, 'r');
    // Black king on h8 (square 63)
    chess_board.set_piece(63, 'k');

    bool is_mate = Piece::is_mate(chess_board, true);

    EXPECT_EQ(is_mate, true);
}
TEST(PieceTest, test_castling) {
    board chess_board;
    chess_board.reset_board();

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // Place only kings and rooks for castling tests
    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(0, 'R');   // White rook a1
    chess_board.set_piece(7, 'R');   // White rook h1

    chess_board.set_piece(60, 'k');  // Black king e8
    chess_board.set_piece(56, 'r');  // Black rook a8
    chess_board.set_piece(63, 'r');  // Black rook h8

    // --- White castling ---
    Bitboard white_castles = Piece::castling(true, chess_board);

    Bitboard expected_white = 0;
    expected_white |= (1ULL << 6);  // g1 (white kingside castle)
    expected_white |= (1ULL << 2);  // c1 (white queenside castle)

    EXPECT_EQ(white_castles, expected_white);

    // --- Black castling ---
    Bitboard black_castles = Piece::castling(false, chess_board);

    Bitboard expected_black = 0;
    expected_black |= (1ULL << 62); // g8 (black kingside castle)
    expected_black |= (1ULL << 58); // c8 (black queenside castle)

    EXPECT_EQ(black_castles, expected_black);
}
TEST(PieceTest, test_castling_with_king_moves) {
    board chess_board;
    chess_board.reset_board();

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // Place only kings and rooks for castling tests
    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(0, 'R');   // White rook a1
    chess_board.set_piece(7, 'R');   // White rook h1

    chess_board.set_piece(60, 'k');  // Black king e8
    chess_board.set_piece(56, 'r');  // Black rook a8
    chess_board.set_piece(63, 'r');  // Black rook h8

    Move move1 = Move(4, 12);
    chess_board.execute_move(move1);
    Move move2 = Move(12, 4);
    chess_board.execute_move(move2);

    // --- White castling ---
    Bitboard white_castles = Piece::castling(true, chess_board);

    Bitboard expected_white = 0ULL;

    EXPECT_EQ(white_castles, expected_white);

}

TEST(PieceTest, test_castling_with_rook_moves) {
    board chess_board;
    chess_board.reset_board();

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // Place only kings and rooks for castling tests
    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(0, 'R');   // White rook a1
    chess_board.set_piece(7, 'R');   // White rook h1

    chess_board.set_piece(60, 'k');  // Black king e8
    chess_board.set_piece(56, 'r');  // Black rook a8
    chess_board.set_piece(63, 'r');  // Black rook h8

    Move move1 = Move(0, 16);
    chess_board.execute_move(move1);
    Move move2 = Move(16, 0);
    chess_board.execute_move(move2);

    // --- White castling ---
    Bitboard white_castles = Piece::castling(true, chess_board);

    Bitboard expected_white = 0ULL;
    expected_white |= (1ULL << 6); //g1

    EXPECT_EQ(white_castles, expected_white);

}

TEST(PieceTest, test_king_moves) { //With castling, without moves
    board chess_board;
    chess_board.reset_board();

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // Place only kings and rooks for castling tests
    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(0, 'R');   // White rook a1
    chess_board.set_piece(7, 'R');   // White rook h1

    chess_board.set_piece(59, 'r');  // Black rook d8

    // --- White castling ---
    Bitboard white_castles = Piece::king_moves(4, true, chess_board);

    Bitboard expected_white = 0;
    expected_white |= (1ULL << 6);  // g1 (white kingside castle)
    expected_white |= (1ULL << 12);  // e2
    expected_white |= (1ULL << 13);  // f2
    expected_white |= (1ULL << 5);  // f1

    EXPECT_EQ(white_castles, expected_white);
}
