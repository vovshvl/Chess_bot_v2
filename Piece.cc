
#include <array>
#include <vector>
#include "board.hh"

class Piece {
private:
    // Bitboard type for representing piece positions and attack patterns
    using Bitboard = uint64_t;

    // Bitboard constants for empty and full boards
    static constexpr Bitboard EMPTY = 0ULL; // All bits set to 0
    static constexpr Bitboard FULL = ~0ULL; // All bits set to 1

    // Basic piece properties
    bool has_moved = false; // Tracks if piece has moved (important for pawns and castling)
    int square; // Current square (0-63, a1=0, h8=63)
    int value; // Piece value (+/- for white/black)
    bool is_white = value > 0;

    // Bitboards for current piece's attack and move patterns
    Bitboard attacks; // Squares this piece attacks
    Bitboard moves; // Legal moves for this piece

    // File masks for move generation
    static constexpr Bitboard FILE_A = 0x0101010101010101ULL; // Leftmost file
    static constexpr Bitboard FILE_B = FILE_A << 1;
    static constexpr Bitboard FILE_G = FILE_A << 6;
    static constexpr Bitboard FILE_H = FILE_A << 7; // Rightmost file

public:

    static bool is_attacked(board& chess_board, int square) {
        // Determine if we're checking for white or black attacks
        bool is_white = chess_board.is_white_piece(square);

        // Get all opponent pieces
        Bitboard opponent_pawns = chess_board.get_pieces_by_value(is_white ? -1 : 1);
        Bitboard opponent_knights = chess_board.get_pieces_by_value(is_white ? -2 : 2);
        Bitboard opponent_bishops = chess_board.get_pieces_by_value(is_white ? -3 : 3);
        Bitboard opponent_rooks = chess_board.get_pieces_by_value(is_white ? -4 : 4);
        Bitboard opponent_queens = chess_board.get_pieces_by_value(is_white ? -5 : 5);
        Bitboard opponent_king = chess_board.get_pieces_by_value(is_white ? -6 : 6);


        Bitboard occupied = chess_board.get_all_pieces();

        if (pawn_attacks(square, !is_white) & opponent_pawns)
            return true;

        if (knight_attacks(square) & opponent_knights)
            return true;

        Bitboard bishop_queens = opponent_bishops | opponent_queens;
        if (bishop_attacks(square, occupied) & bishop_queens)
            return true;

        Bitboard rook_queens = opponent_rooks | opponent_queens;
        if (rook_attacks(square, occupied) & rook_queens)
            return true;
        if (king_attacks(square) & opponent_king)
            return true;

        return false;
    }
    std::vector<std::pair<int,int>> legal_moves(board chess_board, bool color) {
        std::vector<std::pair<int,int>> all_moves;
        if (color) {
            Bitboard knight = chess_board.get_white_knights();
            Bitboard bishop = chess_board.get_white_bishops();
            Bitboard rook = chess_board.get_white_rooks();
            Bitboard queen = chess_board.get_white_queens();
            Bitboard king = chess_board.get_white_king();
            Bitboard pawns = chess_board.get_white_pawns();


        } else {
            Bitboard knight = chess_board.get_black_knights();
            Bitboard bishop = chess_board.get_black_bishops();
            Bitboard rook = chess_board.get_black_rooks();
            Bitboard queen = chess_board.get_black_queens();
            Bitboard king = chess_board.get_black_king();
            Bitboard pawns = chess_board.get_black_pawns();
        }
        if (is_in_check(chess_board, color)) {


        }

        return all_moves;
    }
    static bool is_in_check(board& board, bool white){
        int king_square;
        if(white){
            Bitboard wk = board.get_white_king();
            king_square=__builtin_ctzll(wk);
        }
        else{
            Bitboard bk = board.get_black_king();
            king_square=__builtin_ctzll(bk);
        }
        return is_attacked(board, king_square);
    }
    static void all_legal_moves(board& board, bool white){
        if(white) {
            Bitboard pieces = board.get_white_pieces();
            while (pieces) {
                int square = __builtin_ctzll(pieces); // get index of LSB
                pieces &= pieces - 1;

                char piece = board.get_piece_at_square(square);
            }
        }
    }
    static bool is_mate(board& board, bool white){
        if(is_in_check(board, white)){
            return true;
        }
        return false;
    }

private:

    static Bitboard get_bitboard_by_square(int square) {
        return 1ULL << square;
    }
    static std::vector<int> bitboard_to_array(Bitboard bb) {
        std::vector<int> squares;
        while (bb) {
            int sq = __builtin_ctzll(bb);
            squares.push_back(sq);
            bb &= bb - 1;
        }
        return squares;
    }
    static int get_square_by_bitboard(Bitboard bb) {
        return  __builtin_ctzll(bb);
    }

    static Bitboard pawn_attacks(int sq, bool is_white) {
        Bitboard bb = 1ULL << sq;
        // White pawns attack diagonally upward, black pawns attack diagonally downward
        return is_white
                   ? ((bb << 7) & ~FILE_H) | ((bb << 9) & ~FILE_A)
                   : ((bb >> 7) & ~FILE_A) | ((bb >> 9) & ~FILE_H);
    }

    static Bitboard knight_attacks(int sq) {
        Bitboard bb = 1ULL << sq;
        // Knight's eight possible moves in L-pattern
        return (((bb << 15) | (bb >> 17)) & ~FILE_H) |
               (((bb << 17) | (bb >> 15)) & ~FILE_A) |
               (((bb << 6) | (bb >> 10)) & ~(FILE_G | FILE_H)) |
               (((bb << 10) | (bb >> 6)) & ~(FILE_A | FILE_B));
    }

    static Bitboard bishop_attacks(int sq, Bitboard occupied) {
        Bitboard attacks = EMPTY;
        int r = sq / 8, f = sq % 8;

        for (int dr: {-1, 1}) {
            for (int df: {-1, 1}) {
                int rank = r + dr, file = f + df;
                while (rank >= 0 && rank < 8 && file >= 0 && file < 8) {
                    int target_sq = rank * 8 + file;
                    attacks |= 1ULL << target_sq;
                    if (occupied & (1ULL << target_sq)) break;
                    rank += dr;
                    file += df;
                }
            }
        }
        return attacks;
    }

    static Bitboard rook_attacks(int sq, Bitboard occupied) {
        Bitboard attacks = EMPTY;
        int r = sq / 8, f = sq % 8;
        // Generate attacks in all four orthogonal directions
        for (int d: {-1, 1}) {
            int rank = r + d;
            while (rank >= 0 && rank < 8) {
                int target_sq = rank * 8 + f;
                attacks |= 1ULL << target_sq;
                if (occupied & (1ULL << target_sq)) break;
                rank += d;
            }
            int file = f + d;
            while (file >= 0 && file < 8) {
                int target_sq = r * 8 + file;
                attacks |= 1ULL << target_sq;
                if (occupied & (1ULL << target_sq)) break;
                file += d;
            }
        }
        return attacks;
    }

    static Bitboard king_attacks(int sq) {
        Bitboard bb = 1ULL << sq;

        return ((bb << 1) & ~FILE_A) | ((bb >> 1) & ~FILE_H) |
               (bb << 8) | (bb >> 8) |
               ((bb << 7) & ~FILE_H) | ((bb << 9) & ~FILE_A) |
               ((bb >> 7) & ~FILE_A) | ((bb >> 9) & ~FILE_H);
    }
    static  Bitboard queen_attacks(int sq, Bitboard occupied) {
        return bishop_attacks(sq, occupied) | rook_attacks(sq, occupied);
    }
    static Bitboard pawn_moves(int sq, bool is_white) {
        Bitboard bb = 1ULL << sq;
        // White pawns move forward one square, black pawns move backward one square
        return (is_white
                   ? (bb << 8)
                   : (bb >> 8)) |pawn_attacks(sq, is_white);
    }
    static Bitboard knight_moves(int sq,bool is_white, board& board) {
        Bitboard occupied = board.get_all_pieces();
        return knight_attacks(sq) & ~occupied;
    }
    static Bitboard bishop_moves(int sq, bool is_white, board& board) {
        return bishop_attacks(sq, board.get_all_pieces());
    }
    static Bitboard rook_moves(int sq, bool is_white, board& board) {
        return rook_attacks(sq, board.get_all_pieces());
    }
    static Bitboard queen_moves(int sq, bool is_white, board& board) {
        return queen_attacks(sq, board.get_all_pieces());
    }
    static Bitboard king_moves(int sq, bool is_white, board& board) {
        Bitboard possible_moves= king_attacks(sq);
        for (auto sq: bitboard_to_array(possible_moves)) {
            if (is_white) {

            }
        }

        return possible_moves;
    }
    static Bitboard is_defended(int sq, bool is_white,board chess_board) {
        if (is_white) {

        }
    }
};
