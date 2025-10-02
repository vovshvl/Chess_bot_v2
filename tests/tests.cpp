//
// Created by shaba on 15.08.2025.
//
#include <gtest/gtest.h>
#include "../board.hh"
#include "../Move.hh"
#include "../BestMove.hh"

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
    for (auto& move : demo_moves){
        chess_board.execute_move({move.from, move.to, '\0'});
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

    chess_board.execute_move({12,28, '\0'});

    for(int i=0; i<64; ++i){
        if(i==12){ EXPECT_EQ(chess_board.get_piece_at_square(i), '.'); }
        else if(i==28){ EXPECT_EQ(chess_board.get_piece_at_square(i), 'P');}
        else{
            EXPECT_EQ(chess_board.get_piece_at_square(i),chess_board_before.get_piece_at_square(i));
        }
    }
}

TEST(BoardTest, test_capture){
    board chess_board;

    chess_board.set_piece(28, 'P');
    chess_board.set_piece(35, 'p');

    chess_board.execute_move({28,35});

    board expected_chess_board;

    expected_chess_board.set_piece(35, 'P');
    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), expected_chess_board.get_piece_at_square(i));
    }

}

TEST(BoardTest, test_reverse_move){
    board chess_board;
    chess_board.init_board();

    board chess_board_before=chess_board;

    chess_board.execute_move({12,28, '\0'});
    chess_board.reverse_move({12,28,'\0'});
    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), chess_board_before.get_piece_at_square(i));
    }


}

TEST(BoardTest, test_reverse_move_castling){
    board chess_board;

    chess_board.set_piece(4, 'K');
    chess_board.set_piece(7, 'R');

    board chess_board_before=chess_board;

    chess_board.execute_move({4, 6, '\0'});
    chess_board.reverse_move({4,6,'\0'});

    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), chess_board_before.get_piece_at_square(i));
    }
    EXPECT_EQ(chess_board.get_king_castle_white(), chess_board_before.get_king_castle_white());
}

TEST(BoardTest, test_reverse_move_capture){
    board chess_board;

    chess_board.set_piece(4, 'K');
    chess_board.set_piece(7, 'R');
    chess_board.set_piece(63, 'n');

    board chess_board_before=chess_board;

    chess_board.execute_move({7, 63, '\0'});
    chess_board.reverse_move({7,63,'\0'});

    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), chess_board_before.get_piece_at_square(i));
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
    chess_board.print_different_board(white_moves);
    EXPECT_EQ(white_moves, white_result_moves);
    EXPECT_EQ(black_moves_1, black_result_moves_1);
    EXPECT_EQ(black_moves_2, black_result_moves_2);
}

TEST(PieceTest, test_legal_moves_en_passant){
    board chess_board;

    chess_board.set_piece(36, 'P');
    chess_board.set_piece(51, 'p');

    chess_board.execute_move({51, 35});

    auto pawn_moves = Piece::legal_moves(chess_board, true);
    Move en_passant = {36, 43};

    EXPECT_TRUE(std::find(pawn_moves.begin(), pawn_moves.end(), en_passant) != pawn_moves.end());
}

TEST(BoardTest, test_execute_en_passant){
    board chess_board;

    chess_board.set_piece(36, 'P');
    chess_board.set_piece(51, 'p');

    chess_board.execute_move({51, 35});

    chess_board.execute_move({36, 43});

    board expected_chess_board;

    expected_chess_board.set_piece(43, 'P');
    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), expected_chess_board.get_piece_at_square(i));
    }
}

TEST(BoardTest, test_reverse_en_passant){
    board chess_board;


    chess_board.set_piece(36, 'P');
    chess_board.set_piece(51, 'p');



    chess_board.execute_move({51, 35});

    board copy_chess_board = chess_board;

    chess_board.execute_move({36, 43});

    chess_board.reverse_move({36, 43});

    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), copy_chess_board.get_piece_at_square(i));
    }
    EXPECT_NE(chess_board.en_passant_sq, -1);
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
TEST(PieceTest, test_rook_attack){
    board chess_board;
    Piece p;

    chess_board.set_piece(0, 'r');
    chess_board.set_piece(60, 'r');
    chess_board.set_piece(4, 'K');
    chess_board.set_piece(12, 'R');
    chess_board.set_piece(63, 'k');

    chess_board.print_different_board(p.rook_attacks(60, false, chess_board));
    chess_board.print_different_board(p.rook_attacks(0, false, chess_board));
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

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);

    std::vector<Move> expected_moves = {
            {4, 5, '\0'}, {4, 11, '\0'}, {4, 13, '\0'},
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

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);

    std::vector<Move> expected_moves = {
            {4, 3, '\0'}, {4, 5,  '\0'}, {4, 13,  '\0'}, {11, 12,  '\0'}
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

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);

    std::vector<Move> expected_moves = {
            {4, 3}, {4, 5}
    };

    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);
}

TEST(PieceTest, test_legal_moves_double_check_only_king_moves) {
    board chess_board;

    // White king on e1
    chess_board.set_piece(4, 'K');
    // Black rooks delivering double check from e8 and a1
    chess_board.set_piece(60, 'r');
    chess_board.set_piece(0, 'r');
    // Black king somewhere safe
    chess_board.set_piece(63, 'k');


    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);

    // Only king moves are allowed
    for (auto &move : white_legal_moves) {
        EXPECT_EQ(move.from, 4); // only the king can move
    }
}

TEST(PieceTest, test_king_escape_double_check_with_pinned_pieces) {
    board chess_board;

    // White king on e1
    chess_board.set_piece(4, 'K');
    // White rook pinned on e2
    chess_board.set_piece(12, 'R');
    // Black rooks delivering double check
    chess_board.set_piece(60, 'r');
    chess_board.set_piece(0, 'r');
    // Black king far away
    chess_board.set_piece(63, 'k');

    chess_board.print_different_board(Piece::rook_attacks(60, false, chess_board));
    chess_board.print_different_board(Piece::rook_attacks(0, false, chess_board));

    std::vector<Move> white_moves = Piece::legal_moves(chess_board, true);

    // Only king moves allowed, pinned rook must NOT move
    for (auto &move : white_moves) {
        EXPECT_NE(move.from, 12); // pinned rook cannot move in double check
    }
}



TEST(PieceTest, test_legal_moves_leaving_king_in_check_bishop){
    board chess_board;

    chess_board.set_piece(63, 'r');
    chess_board.set_piece(7, 'K');
    chess_board.set_piece(15, 'B');

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);
    std::vector<Move> expected_moves = {
            {7, 6}, {7, 14}
    };
    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);
}
TEST(PieceTest, test_legal_moves_leaving_king_in_check_queen){
    board chess_board;

    chess_board.set_piece(63, 'r');
    chess_board.set_piece(48, 'r');
    chess_board.set_piece(32, 'r');
    chess_board.set_piece(6, 'r');
    chess_board.set_piece(55, 'Q');
    chess_board.set_piece(47, 'K');

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);
    std::vector<Move> expected_moves = {
            {55, 63}
    };
    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);
}

TEST(PieceTest, test_pinned_piece_blocked) {
    board chess_board;
    chess_board.reset_board();

    // White king on e1
    chess_board.set_piece(4, 'K');
    // White rook pinned on e2
    chess_board.set_piece(12, 'R');
    // Black rook attacking along e-file
    chess_board.set_piece(60, 'r');
    // Black king safe
    chess_board.set_piece(63, 'k');

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);

    // Pinned rook can only move along e-file (capture or stay)
    for (auto &move : white_legal_moves) {
        if (move.from == 12) {
            EXPECT_TRUE(move.to == 20 || move.to == 28 || move.to == 36 || move.to == 44 || move.to == 52 || move.to == 60);
        }
    }
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


    chess_board.execute_move({4,12,  '\0'});
    chess_board.execute_move({12,4,  '\0'});

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

    chess_board.execute_move({0,16,  '\0'});
    chess_board.execute_move({16,0,  '\0'});

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

TEST(PieceTest, test_king_stepping_into_check){
    board chess_board;

    chess_board.set_piece(4, 'K');
    chess_board.set_piece(13, 'P');
    chess_board.set_piece(59, 'r');
    chess_board.set_piece(61, 'r');

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);
    std::vector<Move> expected_moves = {
            {4, 12},{4, 5}, {13, 21}, {13, 29}
    };
    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_EQ(white_legal_moves, expected_moves);

}

TEST(PieceTest, test_king_stepping_into_pawncheck){
    board chess_board;

    chess_board.set_piece(0, 'K');
    chess_board.set_piece(1, 'P');
    chess_board.set_piece(8, 'P');
    chess_board.set_piece(16, 'k');

    chess_board.print_board();

    std::vector<Move> white_legal_moves = Piece::legal_moves(chess_board, true);
    std::vector<Move> expected_moves = {
            {1, 9}
    };
    std::sort(white_legal_moves.begin(), white_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());


    EXPECT_EQ(white_legal_moves, expected_moves);

}

TEST(PieceTest, test_king_stepping_into_rookcheck){
    board chess_board;

    chess_board.set_piece(55, 'k');
    chess_board.set_piece(6, 'R');
    chess_board.set_piece(54, 'Q');

    chess_board.print_board();

    chess_board.white_to_move = false;

    std::vector<Move> black_legal_moves = Piece::legal_moves(chess_board, false);
    std::vector<Move> expected_moves = {
    };
    std::sort(black_legal_moves.begin(), black_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    chess_board.print_different_board(Piece::rook_attacks(6, true, chess_board));

    EXPECT_EQ(black_legal_moves, expected_moves);

}

TEST(PieceTest, test_king_moves_beeing_in_mate_black){
    board chess_board;

    chess_board.set_piece(55, 'K');
    chess_board.set_piece(6, 'r');
    chess_board.set_piece(15, 'r');
    chess_board.set_piece(7, 'k');

    std::vector<Move> black_legal_moves = Piece::legal_moves(chess_board, true);
    //chess_board.white_to_move = false;
    std::vector<Move> expected_moves = {

    };
    std::sort(black_legal_moves.begin(), black_legal_moves.end());
    std::sort(expected_moves.begin(), expected_moves.end());

    EXPECT_TRUE(Piece::is_in_check(chess_board, true));
    EXPECT_TRUE(Piece::is_mate(chess_board, true));
    EXPECT_EQ(black_legal_moves, expected_moves);

}
TEST(PieceTest, test_white_promotion){
    board chess_board;
    chess_board.reset_board();
    Minmax minimax;
    Evaluator eval;

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    chess_board.king_castle_white = false;
    chess_board.king_castle_black = false;
    chess_board.queen_castle_white=false;
    chess_board.queen_castle_black = false;

    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(55, 'k');   // Black king h7
    chess_board.set_piece(49, 'P');   // White pawn b7

    chess_board.execute_move({49, 57, 'q'});

    board expected_board;
    expected_board.set_piece(4, 'K');   // White king e1
    expected_board.set_piece(55, 'k');   // Black king h7
    expected_board.set_piece(57, 'Q');   // White queen b8

    for(int i=0;i<64; ++i){
        EXPECT_EQ(chess_board.get_piece_at_square(i), expected_board.get_piece_at_square(i));
    }
}

TEST(PieceTest, test_pawn_promotion_check) {
    board chess_board;

    // White king safe
    chess_board.set_piece(4, 'k');
    // Black king far away
    chess_board.set_piece(63, 'K');
    // White pawn ready to promote on 6th rank
    chess_board.set_piece(48, 'P');
    // Black rook giving check along the file
    chess_board.set_piece(57, 'r');

    std::vector<Move> legal_moves = Piece::legal_moves(chess_board, true);

    // Pawn promotion must include moves to capture blocking rook
    bool promotion_found = false;
    for (auto &move : legal_moves) {
        if (move.from == 48 && move.to == 57) {
            promotion_found = true;
        }
    }
    EXPECT_TRUE(promotion_found);
}


TEST(BestMoveTest, test_white_promotion){
    board chess_board;
    chess_board.reset_board();
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    chess_board.king_castle_white = false;
    chess_board.king_castle_black = false;
    chess_board.queen_castle_white=false;
    chess_board.queen_castle_black = false;

    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(55, 'k');   // Black king h7
    chess_board.set_piece(49, 'P');   // White pawn b7

    auto best_move = minimax.find_best_move_negamax(chess_board, 5, eval, tt);
    Move expected_move= {49, 57, 'q'};
    EXPECT_EQ(best_move, expected_move);
}

TEST(BestMoveTest, test_black_promotion){
    board chess_board;
    chess_board.reset_board();
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    chess_board.king_castle_white = false;
    chess_board.king_castle_black = false;
    chess_board.queen_castle_white=false;
    chess_board.queen_castle_black = false;

    // Place only kings and rooks for castling tests
    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(55, 'k');   // Black king h7
    chess_board.set_piece(8, 'p');   // Black pawn a2

    chess_board.white_to_move = false;

    auto best_move = minimax.find_best_move_negamax(chess_board, 5, eval, tt);
    Move expected_move= {8, 0, 'q'};
    EXPECT_EQ(best_move, expected_move);
}

TEST(PieceTest, test_white_underpromotion){
    board chess_board;
    chess_board.reset_board();
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    // Clear all pieces
    chess_board.white_pawn = chess_board.black_pawn = 0;
    chess_board.white_knight = chess_board.black_knight = 0;
    chess_board.white_bishop = chess_board.black_bishop = 0;
    chess_board.white_queen = chess_board.black_queen = 0;
    chess_board.white_rook = chess_board.black_rook = 0;
    chess_board.white_king = chess_board.black_king = 0;

    // Place only kings and rooks for castling tests
    chess_board.set_piece(4, 'K');   // White king e1
    chess_board.set_piece(55, 'k');   // Black king h7
    chess_board.set_piece(53, 'P');   // White pawn f7
    chess_board.set_piece(46, 'p');   // Black pawn g6
    chess_board.set_piece(47, 'p');   // Black pawn h6
    chess_board.set_piece(54, 'p');   // Black pawn g7
    chess_board.set_piece(62, 'p');   // Black pawn g8
    chess_board.set_piece(63, 'p');   // Black pawn h8

    auto best_move = minimax.find_best_move_negamax(chess_board, 5, eval, tt);
    Move expected_move= {53, 61, 'n'};
    EXPECT_EQ(best_move, expected_move);
}

TEST(BestMoveTest, test_opening_move) {
    Zobrist::init();
    board chess_board;
    chess_board.init_board();
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    auto best_move = minimax.find_best_move_negamax(chess_board, 6, eval, tt);
    auto white_legal = Piece::legal_moves(chess_board, true);

    // In the initial position, e2-e4 or d2-d4 is likely best. Check that
    // the returned move is in a set of reasonable opening moves
    std::vector<Move> expected_moves = {
            {12, 28}, // e2-e4
            {12, 20}, // e2-e3
            {13, 29}, // d2-d4
            {13, 21}  // d2-d3
    };

    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move) != expected_moves.end());
}

TEST(BestMoveTest, test_opening_moves_black) {
    board chess_board;
    chess_board.init_board();
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.execute_move({12, 28,  '\0'});

    chess_board.white_to_move = false;

    auto best_move = minimax.find_best_move_negamax(chess_board, 4, eval, tt);

    // Black's best moves in response to standard openings
    std::vector<Move> expected_moves = {
            {57, 42}, // b8-c6
            {52, 36}, // e7-e5
            {62, 45}, // g8-f6
            {51,35} //d7-d5
    };

    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move) != expected_moves.end());
}

TEST(BestMoveTest, test_mate_in_one_for_white){ //lichess puzzle #msqFt
    board chess_board;
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.set_piece(9, 'K');
    chess_board.set_piece(10, 'P');
    chess_board.set_piece(17, 'P');
    chess_board.set_piece(8, 'P');
    chess_board.set_piece(25, 'B');
    chess_board.set_piece(43, 'N');
    chess_board.set_piece(55, 'R');

    chess_board.set_piece(59, 'k');
    chess_board.set_piece(46, 'n');
    chess_board.set_piece(22, 'n');
    chess_board.set_piece(14, 'r');
    chess_board.set_piece(7, 'q');
    chess_board.set_piece(36, 'p');

    auto best_move = minimax.find_best_move_negamax(chess_board, 6, eval, tt);
    std::vector<Move> expected_moves = {
            {25, 32}
    };

    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move) != expected_moves.end());
}

TEST(BestMoveTest, test_mate_in_one_for_black){ //lichess puzzle #msqFt
    board chess_board;
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.set_piece(9, 'k');
    chess_board.set_piece(10, 'p');
    chess_board.set_piece(17, 'p');
    chess_board.set_piece(8, 'p');
    chess_board.set_piece(25, 'b');
    chess_board.set_piece(43, 'n');
    chess_board.set_piece(55, 'r');

    chess_board.set_piece(59, 'K');
    chess_board.set_piece(46, 'N');
    chess_board.set_piece(22, 'N');
    chess_board.set_piece(14, 'R');
    chess_board.set_piece(7, 'Q');
    chess_board.set_piece(36, 'P');

    chess_board.white_to_move = false;
    auto best_move = minimax.find_best_move_negamax(chess_board, 4, eval, tt);
    std::vector<Move> expected_moves = {
            {25, 32}
    };

    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move) != expected_moves.end());
}

TEST(BestMoveTest, test_mate_in_one_his_own_game){
    board chess_board;
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.set_piece(55, 'k');
    chess_board.set_piece(39, 'p');
    chess_board.set_piece(53, 'p');
    chess_board.set_piece(51, 'p');
    chess_board.set_piece(48, 'p');
    chess_board.set_piece(40, 'p');
    chess_board.set_piece(36, 'p');
    chess_board.set_piece(60, 'q');
    chess_board.set_piece(61, 'r');
    chess_board.set_piece(26, 'r');

    chess_board.set_piece(5, 'K');
    chess_board.set_piece(6, 'R');
    chess_board.set_piece(12, 'N');
    chess_board.set_piece(6, 'R');
    chess_board.set_piece(0, 'R');
    chess_board.set_piece(45, 'Q');
    chess_board.set_piece(8, 'P');
    chess_board.set_piece(16, 'P');
    chess_board.set_piece(15, 'P');
    chess_board.set_piece(13, 'P');
    chess_board.set_piece(20, 'P');
    chess_board.set_piece(27, 'P');


    auto best_move = minimax.find_best_move_negamax(chess_board, 6, eval, tt);
    std::vector<Move> expected_moves = {
            {45, 54}
    };


    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move) != expected_moves.end());
}

TEST(BestMoveTest, test_castling){
    board chess_board;
    Minmax minimax;
    Evaluator eval;
    chess_board.init_board();

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.execute_move({12,28});
    chess_board.execute_move({52,36});
    chess_board.execute_move({6,21});
    chess_board.execute_move({56,57});
    chess_board.execute_move({5,33});
    chess_board.execute_move({62,45});


    auto best_move = minimax.find_best_move_negamax(chess_board, 4, eval, tt);
    std::vector<Move> expected_moves = {
            {4, 6}
    };


    chess_board.execute_move(best_move);

    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move) != expected_moves.end());

}
TEST(BestMoveTest, test_mate_in_2){
    board chess_board;
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.set_piece(63, 'k');
    chess_board.set_piece(7, 'K');
    chess_board.set_piece(0, 'R');
    chess_board.set_piece(8, 'R');

    chess_board.set_piece(56, 'n');

    auto best_move_1 = minimax.find_best_move_negamax(chess_board, 6, eval, tt);
    auto moves_1 = Piece::legal_moves(chess_board, true );
    Move expected_1 = {0, 6};
    EXPECT_TRUE(std::find(moves_1.begin(), moves_1.end(), expected_1) != moves_1.end());
    chess_board.execute_move(best_move_1);

    auto best_responce = minimax.find_best_move_negamax(chess_board, 4, eval, tt);
    chess_board.execute_move(best_responce);

    auto best_move_2 = minimax.find_best_move_negamax(chess_board, 6, eval, tt);
    auto moves_2 = Piece::legal_moves(chess_board, true );
    Move expected_2 = {8, 15};
    EXPECT_TRUE(std::find(moves_2.begin(), moves_2.end(), expected_2) != moves_2.end());
    chess_board.execute_move(best_move_2);

    std::vector<Move> expected_moves = {
            {0, 6},
            {8, 15}
    };

    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move_1) != expected_moves.end());
    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move_2) != expected_moves.end());

}

TEST(BestMoveTest, test_mate_in_2_position_validity){
    board chess_board;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.set_piece(63, 'k'); // black king h8
    chess_board.set_piece(7, 'K');  // white king h1
    chess_board.set_piece(6, 'R');  // rook a1
    chess_board.set_piece(8, 'R');  // rook a2

    // Verify: White to move, should not already be mate
    EXPECT_FALSE(Piece::is_mate(chess_board, false));
    EXPECT_FALSE(Piece::is_mate(chess_board, true));

    // Play the known first move: Ra1-a8+
    chess_board.execute_move({8,15});
    EXPECT_TRUE(Piece::is_in_check(chess_board, false));

    // Enumerate black replies
    auto black_moves = Piece::legal_moves(chess_board, false);
    for(auto &m : black_moves){
        board tmp = chess_board;
        tmp.execute_move(m);
        bool is_mate = Piece::is_mate(tmp, false);
        EXPECT_TRUE(is_mate); // All black replies should lose
    }
}

TEST(BestMoveTest, mate_score_depth_sensitive) {
    board chess_board;
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    // Simple mate-in-1
    chess_board.set_piece(59, 'k'); // black king e8
    chess_board.set_piece(60, 'K'); // white king e1
    chess_board.set_piece(55, 'Q'); // white queen h1 -> Qh5#

    int score_m1 = minimax.negamax(chess_board, 1, -1000000, 1000000, eval, true, 0, tt);
    int score_m2 = minimax.negamax(chess_board, 2, -1000000, 1000000, eval, true, 0 ,tt);

    EXPECT_GT(score_m1, score_m2); // mate in 1 must be valued higher than mate in 2
}


/*
TEST(BestMoveTest, test_mate_in_3){
    board chess_board;
    Minmax minimax;
    Evaluator eval;

    size_t tt_size = 1 << 20; // 1M entries
    TranspositionTable tt(tt_size);

    chess_board.set_piece(63, 'k');
    chess_board.set_piece(62, 'r');
    chess_board.set_piece(56, 'r');
    chess_board.set_piece(55, 'p');
    chess_board.set_piece(53, 'p');

    chess_board.set_piece(45, 'R');
    chess_board.set_piece(36, 'B');
    chess_board.set_piece(15, 'P');
    chess_board.set_piece(7, 'K');


    chess_board.print_board();

    std::cout<< "best move 1 " << "\n";
    auto best_move_1 = minimax.find_best_move_negamax(chess_board, 8, eval, tt);
    auto moves_1 = Piece::legal_moves(chess_board, true );
    Move expected_1 = {45, 40};
    EXPECT_TRUE(std::find(moves_1.begin(), moves_1.end(), expected_1) != moves_1.end());
    chess_board.execute_move(best_move_1);
    chess_board.print_board();

    std::cout<< "best response 1 " << "\n";
    auto best_response = minimax.find_best_move_negamax(chess_board, 4, eval, tt);
    chess_board.execute_move(best_response);
    chess_board.print_board();

    std::cout<< "best move 2 " << "\n";
    auto best_move_2 = minimax.find_best_move_negamax(chess_board, 8, eval, tt);
    auto moves_2 = Piece::legal_moves(chess_board, true );
    Move expected_2 = {36, 45};
    EXPECT_TRUE(std::find(moves_2.begin(), moves_2.end(), expected_2) != moves_2.end());
    chess_board.execute_move(best_move_2);
    chess_board.print_board();

    std::cout<< "best response 2 " << "\n";
    auto best_response_2 = minimax.find_best_move_negamax(chess_board, 4, eval, tt);
    chess_board.execute_move(best_response_2);
    chess_board.print_board();

    std::cout<< "best move 3 " << "\n";
    auto best_move_3 = minimax.find_best_move_negamax(chess_board, 8, eval, tt);
    auto moves_3 = Piece::legal_moves(chess_board, true );
    Move expected_3 = {40, 56};
    EXPECT_TRUE(std::find(moves_3.begin(), moves_3.end(), expected_3) != moves_3.end());
    chess_board.execute_move(best_move_3);
    chess_board.print_board();

    std::vector<Move> expected_moves = {
            {45, 40},
            {36, 45},
            {40, 56}
    };

    auto resp = minimax.find_best_move_negamax(chess_board, 4, eval, tt);
    chess_board.execute_move(resp);
    chess_board.print_board();


    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move_1) != expected_moves.end());
    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move_2) != expected_moves.end());
    EXPECT_TRUE(std::find(expected_moves.begin(), expected_moves.end(), best_move_3) != expected_moves.end());

    //Fix because he can move pinned piece
    //EXPECT_TRUE(Piece::is_mate(chess_board, false));

}
*/
