

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
    bool is_attacked(board& board, int nextsquare ) const {
        // Get opponent piece positions

        Bitboard opponent_pawns = board.get_piece_bitboard(is_white ? -1 : 1);
        Bitboard opponent_knights = board.get_piece_bitboard(is_white ? -2 : 2);
        Bitboard opponent_bishops = board.get_piece_bitboard(is_white ? -3 : 3);
        Bitboard opponent_rooks = board.get_piece_bitboard(is_white ? -4 : 4);
        Bitboard opponent_queens = board.get_piece_bitboard(is_white ? -5 : 5);
        Bitboard opponent_king = board.get_piece_bitboard(is_white ? -6 : 6);

        // Check attacks from all piece types
        return (pawn_attacks(square, !is_white) & opponent_pawns) ||
               (knight_attacks(square) & opponent_knights) ||
               (bishop_attacks(square, board.get_occupancy()) & (opponent_bishops | opponent_queens)) ||
               (rook_attacks(square, board.get_occupancy()) & (opponent_rooks | opponent_queens)) ||
               (king_attacks(square) & opponent_king);
    }

private:

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
    static  Bitboard king_moves(int sq) {
        return king_attacks(sq);

    }
};
