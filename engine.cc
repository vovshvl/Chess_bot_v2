#include "engine.hh"
#include "board.hh"




// Piece values
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;

// Piece-square tables
const int PAWN_TABLE[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5, 5, 10, 25, 25, 10, 5, 5,
    0, 0, 0, 20, 20, 0, 0, 0,
    5, -5, -10, 0, 0, -10, -5, 5,
    5, 10, 10, -20, -20, 10, 10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
};

const int KNIGHT_TABLE[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

int evaluate_board(board chess_board) {
    int score = 0;

    for (int i = 0; i < 64; i++) {
        char piece = chess_board.get_piece_at_square(i);
        switch (piece) {
            case 'P': score += PAWN_VALUE + PAWN_TABLE[i];
                break;
            case 'p': score -= PAWN_VALUE + PAWN_TABLE[63 - i];
                break;
            case 'N': score += KNIGHT_VALUE + KNIGHT_TABLE[i];
                break;
            case 'n': score -= KNIGHT_VALUE + KNIGHT_TABLE[63 - i];
                break;
            case 'B': score += BISHOP_VALUE;
                break;
            case 'b': score -= BISHOP_VALUE;
                break;
            case 'R': score += ROOK_VALUE;
                break;
            case 'r': score -= ROOK_VALUE;
                break;
            case 'Q': score += QUEEN_VALUE;
                break;
            case 'q': score -= QUEEN_VALUE;
                break;
            case 'K': score += KING_VALUE;
                break;
            case 'k': score -= KING_VALUE;
                break;
        }
    }

    return score;
}
