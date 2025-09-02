//
// Created by Kirill on 9/1/2025.
//

#ifndef CHESS_BOT_NEW_ENGINE_HH

#define CHESS_BOT_NEW_ENGINE_HH
#include <array>
#include <algorithm>
#include "board.hh"
#include <cstdint>

class Evaluator {
private:
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr std::array<int16_t, 64> PAWN_PST = {
         0,  0,  0,  0,  0,  0,  0,  0,
        78, 83, 86, 73, 102, 82, 85, 90,
         7, 29, 21, 44, 40, 31, 44, 7,
        -17, 16, -2, 15, 14, 0, 15, -13,
        -26, 3, 10, 9, 6, 1, 0, -23,
        -22, 9, 5, -11, -10, -2, 3, -19,
        -31, 8, -7, -37, -36, -14, 3, -31,
         0, 0, 0, 0, 0, 0, 0, 0
    };

    static constexpr std::array<int16_t, 64> KNIGHT_PST = {
        -66, -53, -75, -75, -10, -55, -58, -70,
        -3, -6, 100, -36, 4, 62, -4, -14,
        10, 67, 1, 74, 73, 27, 62, -2,
        24, 24, 45, 37, 33, 41, 25, 17,
        -1, 5, 31, 21, 22, 35, 2, 0,
        -18, 10, 13, 22, 18, 15, 11, -14,
        -23, -15, 2, 0, 2, 0, -23, -20,
        -74, -23, -26, -24, -19, -35, -22, -69
    };

    static constexpr std::array<int16_t, 64> BISHOP_PST = {
        -59, -78, -82, -76, -23, -107, -37, -50,
        -11, 20, 35, -42, -39, 31, 2, -22,
        -9, 39, -32, 41, 52, -10, 28, -14,
        25, 17, 20, 34, 26, 25, 15, 10,
        13, 10, 17, 23, 17, 16, 0, 7,
        14, 25, 24, 15, 8, 25, 20, 15,
        19, 20, 11, 6, 7, 6, 20, 16,
        -7, 2, -15, -12, -14, -15, -10, -10
    };

    static constexpr std::array<int16_t, 64> ROOK_PST = {
        35, 29, 33, 4, 37, 33, 56, 50,
        55, 29, 56, 67, 55, 62, 34, 60,
        19, 35, 28, 33, 45, 27, 25, 15,
        0, 5, 16, 13, 18, -4, -9, -6,
        -28, -35, -16, -21, -13, -29, -46, -30,
        -42, -28, -42, -25, -25, -35, -26, -46,
        -53, -38, -31, -26, -29, -43, -44, -53,
        -30, -24, -18, 5, -2, -18, -31, -32
    };

    static constexpr std::array<int16_t, 64> QUEEN_PST = {
        6, 1, -8, -104, 69, 24, 88, 26,
        14, 32, 60, -10, 20, 76, 57, 24,
        -2, 43, 32, 60, 72, 63, 43, 2,
        1, -16, 22, 17, 25, 20, -13, -6,
        -14, -15, -2, -5, -1, -10, -20, -22,
        -30, -6, -13, -11, -16, -11, -16, -27,
        -36, -18, 0, -19, -15, -8, -15, -21,
        -39, -30, -31, -13, -31, -36, -34, -42
    };

    static constexpr std::array<int16_t, 64> KING_MG_PST = {
        4, 54, 47, -99, -99, 60, 83, -62,
        -32, 10, 55, 56, 56, 55, 10, 3,
        -62, 12, -57, 44, -67, 28, 37, -31,
        -55, 50, 11, -4, -19, 13, 0, -49,
        -55, -43, -52, -28, -51, -47, -8, -50,
        -47, -42, -43, -79, -64, -32, -29, -32,
        -4, 3, -14, -50, -57, -18, 13, 4,
        17, 30, -3, -14, 6, -1, 40, 18
    };

    static constexpr std::array<int16_t, 64> KING_EG_PST = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
    };

    // Pre-computed masks for ultra-fast lookups
    static constexpr std::array<uint64_t, 8> FILE_MASKS = {
        0x0101010101010101ULL, 0x0202020202020202ULL, 0x0404040404040404ULL,
        0x0808080808080808ULL, 0x1010101010101010ULL, 0x2020202020202020ULL,
        0x4040404040404040ULL, 0x8080808080808080ULL
    };

    // King safety masks - precomputed for maximum speed
    static constexpr std::array<uint64_t, 64> KING_ZONE_MASKS = {
        0x0000000000000302ULL, 0x0000000000000705ULL, 0x0000000000000E0AULL, 0x0000000000001C14ULL,
        0x0000000000003828ULL, 0x0000000000007050ULL, 0x000000000000E0A0ULL, 0x000000000000C040ULL,
        0x0000000000030203ULL, 0x0000000000070507ULL, 0x00000000000E0A0EULL, 0x00000000001C141CULL,
        0x0000000000382838ULL, 0x0000000000705070ULL, 0x0000000000e0a0e0ULL, 0x0000000000c040c0ULL,
        0x0000000302030000ULL, 0x0000000705070000ULL, 0x0000000e0a0e0000ULL, 0x0000001c141c0000ULL,
        0x0000003828380000ULL, 0x0000007050700000ULL, 0x000000e0a0e00000ULL, 0x000000c040c00000ULL,
        0x0000030203000000ULL, 0x0000070507000000ULL, 0x00000e0a0e000000ULL, 0x00001c141c000000ULL,
        0x0000382838000000ULL, 0x0000705070000000ULL, 0x0000e0a0e0000000ULL, 0x0000c040c0000000ULL,
        0x0003020300000000ULL, 0x0007050700000000ULL, 0x000e0a0e00000000ULL, 0x001c141c00000000ULL,
        0x0038283800000000ULL, 0x0070507000000000ULL, 0x00e0a0e000000000ULL, 0x00c040c000000000ULL,
        0x0302030000000000ULL, 0x0705070000000000ULL, 0x0e0a0e0000000000ULL, 0x1c141c0000000000ULL,
        0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL,
        0x0203000000000000ULL, 0x0507000000000000ULL, 0x0a0e000000000000ULL, 0x141c000000000000ULL,
        0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL
    };

    // Pawn shield masks for each king position
    static constexpr std::array<uint64_t, 64> PAWN_SHIELD_MASKS = {
        0x0000000000000300ULL, 0x0000000000000700ULL, 0x0000000000000e00ULL, 0x0000000000001c00ULL,
        0x0000000000003800ULL, 0x0000000000007000ULL, 0x000000000000e000ULL, 0x000000000000c000ULL,
        0x0000000000030000ULL, 0x0000000000070000ULL, 0x00000000000e0000ULL, 0x00000000001c0000ULL,
        0x0000000000380000ULL, 0x0000000000700000ULL, 0x0000000000e00000ULL, 0x0000000000c00000ULL,
        0x0000000003000000ULL, 0x0000000007000000ULL, 0x000000000e000000ULL, 0x000000001c000000ULL,
        0x0000000038000000ULL, 0x0000000070000000ULL, 0x00000000e0000000ULL, 0x00000000c0000000ULL,
        0x0000000300000000ULL, 0x0000000700000000ULL, 0x0000000e00000000ULL, 0x0000001c00000000ULL,
        0x0000003800000000ULL, 0x0000007000000000ULL, 0x000000e000000000ULL, 0x000000c000000000ULL,
        0x0000030000000000ULL, 0x0000070000000000ULL, 0x00000e0000000000ULL, 0x00001c0000000000ULL,
        0x0000380000000000ULL, 0x0000700000000000ULL, 0x0000e00000000000ULL, 0x0000c00000000000ULL,
        0x0003000000000000ULL, 0x0007000000000000ULL, 0x000e000000000000ULL, 0x001c000000000000ULL,
        0x0038000000000000ULL, 0x0070000000000000ULL, 0x00e0000000000000ULL, 0x00c0000000000000ULL,
        0x0300000000000000ULL, 0x0700000000000000ULL, 0x0e00000000000000ULL, 0x1c00000000000000ULL,
        0x3800000000000000ULL, 0x7000000000000000ULL, 0xe000000000000000ULL, 0xc000000000000000ULL,
        0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL,
        0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL
    };

    static inline int popcount(uint64_t x) noexcept {
        return __builtin_popcountll(x);
    }
    //must be checked for being nonzero
    static inline int lsb(uint64_t x) noexcept {
        return __builtin_ctzll(x);
    }

    static inline uint64_t clear_lsb(uint64_t x) noexcept {
        return x & (x - 1);
    }

    static inline bool is_endgame(const board& b) noexcept {
        uint64_t heavy_pieces = b.get_white_queens() | b.get_black_queens() |
                               b.get_white_rooks() | b.get_black_rooks();
        return popcount(heavy_pieces) <= 4; // Queens + rooks <= 4 pieces
    }

    int evaluate_material(const board& b) const noexcept {
        int score = 0;
        score += popcount(b.get_white_pawns()) * PAWN_VALUE;
        score -= popcount(b.get_black_pawns()) * PAWN_VALUE;
        score += popcount(b.get_white_knights()) * KNIGHT_VALUE;
        score -= popcount(b.get_black_knights()) * KNIGHT_VALUE;
        score += popcount(b.get_white_bishops()) * BISHOP_VALUE;
        score -= popcount(b.get_black_bishops()) * BISHOP_VALUE;
        score += popcount(b.get_white_rooks()) * ROOK_VALUE;
        score -= popcount(b.get_black_rooks()) * ROOK_VALUE;
        score += popcount(b.get_white_queens()) * QUEEN_VALUE;
        score -= popcount(b.get_black_queens()) * QUEEN_VALUE;

        return score;
    }

    int evaluate_pst(const board& b, bool endgame) const noexcept {
        int score = 0;
        uint64_t pieces = b.get_white_pawns();
        while (pieces) {
            score += PAWN_PST[lsb(pieces)];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_white_knights();
        while (pieces) {
            score += KNIGHT_PST[lsb(pieces)];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_white_bishops();
        while (pieces) {
            score += BISHOP_PST[lsb(pieces)];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_white_rooks();
        while (pieces) {
            score += ROOK_PST[lsb(pieces)];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_white_queens();
        while (pieces) {
            score += QUEEN_PST[lsb(pieces)];
            pieces = clear_lsb(pieces);
        }
        pieces = b.get_white_king();
        if (pieces) {
            score += endgame ? KING_EG_PST[lsb(pieces)] : KING_MG_PST[lsb(pieces)];
        }

        pieces = b.get_black_pawns();
        while (pieces) {
            score -= PAWN_PST[lsb(pieces) ^ 56];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_black_knights();
        while (pieces) {
            score -= KNIGHT_PST[lsb(pieces) ^ 56];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_black_bishops();
        while (pieces) {
            score -= BISHOP_PST[lsb(pieces) ^ 56];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_black_rooks();
        while (pieces) {
            score -= ROOK_PST[lsb(pieces) ^ 56];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_black_queens();
        while (pieces) {
            score -= QUEEN_PST[lsb(pieces) ^ 56];
            pieces = clear_lsb(pieces);
        }

        pieces = b.get_black_king();
        if (pieces) {
            score -= endgame ? KING_EG_PST[lsb(pieces) ^ 56] : KING_MG_PST[lsb(pieces) ^ 56];
        }

        return score;
    }

    int evaluate_pawn_structure(const board& b) const noexcept {
        int score = 0;
        uint64_t white_pawns = b.get_white_pawns();
        uint64_t black_pawns = b.get_black_pawns();

        for (int file = 0; file < 8; file++) {
            int white_count = popcount(white_pawns & FILE_MASKS[file]);
            int black_count = popcount(black_pawns & FILE_MASKS[file]);

            if (white_count > 1) score -= 15 * (white_count - 1);
            if (black_count > 1) score += 15 * (black_count - 1);
        }

        return score;
    }

    int evaluate_piece_bonuses(const board& b) const noexcept {
        int score = 0;

        // Bishop pair
        if (popcount(b.get_white_bishops()) >= 2) score += 30;
        if (popcount(b.get_black_bishops()) >= 2) score -= 30;

        return score;
    }

    int evaluate_king_safety(const board& b, bool endgame) const noexcept {
        if (endgame) return 0;

        int score = 0;

        uint64_t white_king = b.get_white_king();
        if (white_king) {
            int king_sq = lsb(white_king);

            uint64_t pawn_shield = b.get_white_pawns() & PAWN_SHIELD_MASKS[king_sq];
            int shield_pawns = popcount(pawn_shield);
            score += shield_pawns * 15;

            uint64_t enemy_pieces = b.get_black_queens() | b.get_black_rooks();
            if (enemy_pieces) {
                uint64_t king_zone = KING_ZONE_MASKS[king_sq];
                int attackers = popcount(enemy_pieces & king_zone);
                score -= attackers * 25;
            }

            int king_file = king_sq % 8;
            for (int f = std::max(0, king_file - 1); f <= std::min(7, king_file + 1); f++) {
                if (!(b.get_white_pawns() & FILE_MASKS[f])) {
                    score -= 20;
                    if (b.get_black_rooks() & FILE_MASKS[f]) {
                        score -= 15;
                    }
                }
            }
        }

        uint64_t black_king = b.get_black_king();
        if (black_king) {
            int king_sq = lsb(black_king);

            uint64_t pawn_shield = b.get_black_pawns() & PAWN_SHIELD_MASKS[king_sq ^ 56];
            int shield_pawns = popcount(pawn_shield);
            score -= shield_pawns * 15;

            uint64_t enemy_pieces = b.get_white_queens() | b.get_white_rooks();
            if (enemy_pieces) {
                uint64_t king_zone = KING_ZONE_MASKS[king_sq];
                int attackers = popcount(enemy_pieces & king_zone);
                score += attackers * 25;
            }
            int king_file = king_sq % 8;
            for (int f = std::max(0, king_file - 1); f <= std::min(7, king_file + 1); f++) {
                if (!(b.get_black_pawns() & FILE_MASKS[f])) {
                    score += 20;
                    if (b.get_white_rooks() & FILE_MASKS[f]) {
                        score += 15;
                    }
                }
            }
        }

        return score;
    }

public:
    int evaluate(const board& b, bool white_to_move = true) const noexcept {
        bool endgame = is_endgame(b);

        int score = evaluate_material(b);
        score += evaluate_pst(b, endgame);
        score += evaluate_piece_bonuses(b);
        score += evaluate_king_safety(b, endgame);

        if (!endgame) {
            score += evaluate_pawn_structure(b);
        }

        score += 10;

        return score;
    }
};
extern Evaluator g_ultra_evaluator;
int evaluate_position(const board& b, bool white_to_move = true) noexcept;

#endif //CHESS_BOT_NEW_ENGINE_HH