#include <array>
#include <algorithm>
#include <bit>
#include "board.hh"

class OptimizedEvaluator {
private:
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;

    // Piece-square tables (from white's perspective, a1=0)
    static constexpr std::array<int, 64> PAWN_PST = {
         0,  0,  0,  0,  0,  0,  0,  0,
        78, 83, 86, 73, 102, 82, 85, 90,
         7, 29, 21, 44, 40, 31, 44, 7,
        -17, 16, -2, 15, 14, 0, 15, -13,
        -26, 3, 10, 9, 6, 1, 0, -23,
        -22, 9, 5, -11, -10, -2, 3, -19,
        -31, 8, -7, -37, -36, -14, 3, -31,
         0, 0, 0, 0, 0, 0, 0, 0
    };

    static constexpr std::array<int, 64> KNIGHT_PST = {
        -66, -53, -75, -75, -10, -55, -58, -70,
        -3, -6, 100, -36, 4, 62, -4, -14,
        10, 67, 1, 74, 73, 27, 62, -2,
        24, 24, 45, 37, 33, 41, 25, 17,
        -1, 5, 31, 21, 22, 35, 2, 0,
        -18, 10, 13, 22, 18, 15, 11, -14,
        -23, -15, 2, 0, 2, 0, -23, -20,
        -74, -23, -26, -24, -19, -35, -22, -69
    };

    static constexpr std::array<int, 64> BISHOP_PST = {
        -59, -78, -82, -76, -23, -107, -37, -50,
        -11, 20, 35, -42, -39, 31, 2, -22,
        -9, 39, -32, 41, 52, -10, 28, -14,
        25, 17, 20, 34, 26, 25, 15, 10,
        13, 10, 17, 23, 17, 16, 0, 7,
        14, 25, 24, 15, 8, 25, 20, 15,
        19, 20, 11, 6, 7, 6, 20, 16,
        -7, 2, -15, -12, -14, -15, -10, -10
    };

    static constexpr std::array<int, 64> ROOK_PST = {
        35, 29, 33, 4, 37, 33, 56, 50,
        55, 29, 56, 67, 55, 62, 34, 60,
        19, 35, 28, 33, 45, 27, 25, 15,
        0, 5, 16, 13, 18, -4, -9, -6,
        -28, -35, -16, -21, -13, -29, -46, -30,
        -42, -28, -42, -25, -25, -35, -26, -46,
        -53, -38, -31, -26, -29, -43, -44, -53,
        -30, -24, -18, 5, -2, -18, -31, -32
    };

    static constexpr std::array<int, 64> QUEEN_PST = {
        6, 1, -8, -104, 69, 24, 88, 26,
        14, 32, 60, -10, 20, 76, 57, 24,
        -2, 43, 32, 60, 72, 63, 43, 2,
        1, -16, 22, 17, 25, 20, -13, -6,
        -14, -15, -2, -5, -1, -10, -20, -22,
        -30, -6, -13, -11, -16, -11, -16, -27,
        -36, -18, 0, -19, -15, -8, -15, -21,
        -39, -30, -31, -13, -31, -36, -34, -42
    };

    static constexpr std::array<int, 64> KING_MG_PST = {
        4, 54, 47, -99, -99, 60, 83, -62,
        -32, 10, 55, 56, 56, 55, 10, 3,
        -62, 12, -57, 44, -67, 28, 37, -31,
        -55, 50, 11, -4, -19, 13, 0, -49,
        -55, -43, -52, -28, -51, -47, -8, -50,
        -47, -42, -43, -79, -64, -32, -29, -32,
        -4, 3, -14, -50, -57, -18, 13, 4,
        17, 30, -3, -14, 6, -1, 40, 18
    };

    static constexpr std::array<int, 64> KING_EG_PST = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
    };

    // Flip square for black pieces
    static constexpr int flip_square(int sq) {
        return sq ^ 56;
    }

    static int get_pst_value(int piece_type, int square, bool is_white, bool endgame = false) {
        int sq = is_white ? square : flip_square(square);

        switch(piece_type) {
            case 1: return PAWN_PST[sq];  // PAWN
            case 2: return KNIGHT_PST[sq]; // KNIGHT
            case 3: return BISHOP_PST[sq]; // BISHOP
            case 4: return ROOK_PST[sq];   // ROOK
            case 5: return QUEEN_PST[sq];  // QUEEN
            case 6: return endgame ? KING_EG_PST[sq] : KING_MG_PST[sq]; // KING
            default: return 0;
        }
    }

    // Fast popcount using builtin
    static int popcount(uint64_t bb) {
        return std::popcount(bb);
    }

    // Get least significant bit
    static int lsb(uint64_t bb) {
        return std::countr_zero(bb);
    }

    // Evaluate material and PST for a piece type
    int evaluate_piece_type(uint64_t white_pieces, uint64_t black_pieces,
                           int piece_value, int piece_type, bool endgame) const {
        int score = 0;

        // Material
        score += popcount(white_pieces) * piece_value;
        score -= popcount(black_pieces) * piece_value;

        // Position values
        uint64_t white_copy = white_pieces;
        while (white_copy) {
            int sq = lsb(white_copy);
            score += get_pst_value(piece_type, sq, true, endgame);
            white_copy &= white_copy - 1;
        }

        uint64_t black_copy = black_pieces;
        while (black_copy) {
            int sq = lsb(black_copy);
            score -= get_pst_value(piece_type, sq, false, endgame);
            black_copy &= black_copy - 1;
        }

        return score;
    }

    // Evaluate pawn structure using bitboards
    int evaluate_pawn_structure(const board& b) const {
        uint64_t white_pawns = b.get_white_pawns();
        uint64_t black_pawns = b.get_black_pawns();
        int score = 0;

        // File masks
        static constexpr uint64_t FILE_MASKS[8] = {
            0x0101010101010101ULL, 0x0202020202020202ULL, 0x0404040404040404ULL,
            0x0808080808080808ULL, 0x1010101010101010ULL, 0x2020202020202020ULL,
            0x4040404040404040ULL, 0x8080808080808080ULL
        };

        for (int file = 0; file < 8; file++) {
            uint64_t file_mask = FILE_MASKS[file];

            int white_pawns_on_file = popcount(white_pawns & file_mask);
            int black_pawns_on_file = popcount(black_pawns & file_mask);

            // Doubled pawns penalty
            if (white_pawns_on_file > 1) score -= 10 * (white_pawns_on_file - 1);
            if (black_pawns_on_file > 1) score += 10 * (black_pawns_on_file - 1);

            // Isolated pawns
            uint64_t adjacent_files = 0;
            if (file > 0) adjacent_files |= FILE_MASKS[file - 1];
            if (file < 7) adjacent_files |= FILE_MASKS[file + 1];

            if (white_pawns_on_file > 0 && !(white_pawns & adjacent_files)) {
                score -= 15; // Isolated pawn penalty
            }
            if (black_pawns_on_file > 0 && !(black_pawns & adjacent_files)) {
                score += 15;
            }

            // Passed pawns (simplified check)
            if (white_pawns_on_file == 1 && !black_pawns_on_file) {
                uint64_t white_pawn_sq = white_pawns & file_mask;
                int rank = lsb(white_pawn_sq) / 8;
                if (rank >= 4) score += 20 + (rank - 4) * 10; // Bonus increases closer to promotion
            }
            if (black_pawns_on_file == 1 && !white_pawns_on_file) {
                uint64_t black_pawn_sq = black_pawns & file_mask;
                int rank = lsb(black_pawn_sq) / 8;
                if (rank <= 3) score -= 20 + (3 - rank) * 10;
            }
        }

        return score;
    }

    // Evaluate king safety using bitboard patterns
    int evaluate_king_safety(const board& b, bool endgame) const {
        if (endgame) return 0; // King safety less important in endgame

        int score = 0;
        uint64_t white_king = b.get_white_king();
        uint64_t black_king = b.get_black_king();
        uint64_t white_pawns = b.get_white_pawns();
        uint64_t black_pawns = b.get_black_pawns();

        // Pawn shield masks (simplified)
        static constexpr uint64_t WHITE_KING_SHIELD = 0x0000000000070000ULL; // f2, g2, h2
        static constexpr uint64_t BLACK_KING_SHIELD = 0x0007000000000000ULL; // f7, g7, h7

        if (white_king & 0x00000000000000F0ULL) { // King on back rank
            int shield_pawns = popcount(white_pawns & WHITE_KING_SHIELD);
            score += shield_pawns * 15; // Bonus for pawn shield
            score -= (3 - shield_pawns) * 20; // Penalty for broken shield
        } else {
            score -= 30; // Penalty for exposed king
        }

        if (black_king & 0xF000000000000000ULL) { // King on back rank
            int shield_pawns = popcount(black_pawns & BLACK_KING_SHIELD);
            score -= shield_pawns * 15;
            score += (3 - shield_pawns) * 20;
        } else {
            score += 30;
        }

        return score;
    }

    // Fast endgame detection
    bool is_endgame(const board& b) const {
        int piece_count = popcount(b.get_all_pieces()) - popcount(b.get_white_pawns()) - popcount(b.get_black_pawns()) - 2; // Exclude pawns and kings
        int material = popcount(b.get_white_queens() | b.get_black_queens()) * 9 +
                      popcount(b.get_white_rooks() | b.get_black_rooks()) * 5 +
                      popcount(b.get_white_bishops() | b.get_black_bishops()) * 3 +
                      popcount(b.get_white_knights() | b.get_black_knights()) * 3;

        return piece_count <= 6 || material <= 13;
    }

public:
    // Main evaluation function - returns score from white's perspective
    int evaluate(const board& b) const {
        bool endgame = is_endgame(b);
        int score = 0;

        // Material and piece-square tables
        score += evaluate_piece_type(b.get_white_pawns(), b.get_black_pawns(), PAWN_VALUE, 1, endgame);
        score += evaluate_piece_type(b.get_white_knights(), b.get_black_knights(), KNIGHT_VALUE, 2, endgame);
        score += evaluate_piece_type(b.get_white_bishops(), b.get_black_bishops(), BISHOP_VALUE, 3, endgame);
        score += evaluate_piece_type(b.get_white_rooks(), b.get_black_rooks(), ROOK_VALUE, 4, endgame);
        score += evaluate_piece_type(b.get_white_queens(), b.get_black_queens(), QUEEN_VALUE, 5, endgame);
        score += evaluate_piece_type(b.get_white_king(), b.get_black_king(), 0, 6, endgame);

        // Bishop pair bonus
        if (popcount(b.get_white_bishops()) >= 2) score += 30;
        if (popcount(b.get_black_bishops()) >= 2) score -= 30;

        // Rook bonuses
        uint64_t all_pawns = b.get_white_pawns() | b.get_black_pawns();
        for (int file = 0; file < 8; file++) {
            uint64_t file_mask = 0x0101010101010101ULL << file;
            if (!(all_pawns & file_mask)) { // Open file
                if (b.get_white_rooks() & file_mask) score += 25;
                if (b.get_black_rooks() & file_mask) score -= 25;
            }
        }

        // Pawn structure
        score += evaluate_pawn_structure(b);

        // King safety
        score += evaluate_king_safety(b, endgame);

        // Tempo bonus (slight advantage for side to move - assume white for now)
        score += 10;

        return score;
    }

};
static OptimizedEvaluator g_evaluator;

int evaluate_position(const board& b) {
    return g_evaluator.evaluate(b);
}