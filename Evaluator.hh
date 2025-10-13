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
public:
    static inline int popcount(uint64_t x) noexcept {
        return __builtin_popcountll(x);
    }

    static inline int lsb(uint64_t x) noexcept {
        return __builtin_ctzll(x);
    }

    static inline uint64_t clear_lsb(uint64_t x) noexcept {
        return x & (x - 1);
    }
private:
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;

    static constexpr std::array<int16_t, 64> PAWN_PST = {
         0,   0,   0,   0,   0,   0,   0,   0,  // Rank 1
        50,  50,  50,  50,  50,  50,  50,  50,  // Rank 2
        10,  10,  20,  30,  30,  20,  10,  10,  // Rank 3
         5,   5,  10,  25,  25,  10,   5,   5,  // Rank 4
         0,   0,   0,  20,  20,   0,   0,   0,  // Rank 5
         5,  -5, -10,   0,   0, -10,  -5,   5,  // Rank 6
         5,  10,  10, -20, -20,  10,  10,   5,  // Rank 7
         0,   0,   0,   0,   0,   0,   0,   0   // Rank 8
    };

    static constexpr std::array<int16_t, 64> KNIGHT_PST = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20,   0,   0,   0,   0, -20, -40,
        -30,   0,  10,  15,  15,  10,   0, -30,
        -30,   5,  15,  20,  20,  15,   5, -30,
        -30,   0,  15,  20,  20,  15,   0, -30,
        -30,   5,  10,  15,  15,  10,   5, -30,
        -40, -20,   0,   5,   5,   0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    };

    static constexpr std::array<int16_t, 64> BISHOP_PST = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -10,   0,   5,  10,  10,   5,   0, -10,
        -10,   5,   5,  10,  10,   5,   5, -10,
        -10,   0,  10,  10,  10,  10,   0, -10,
        -10,  10,  10,  10,  10,  10,  10, -10,
        -10,   5,   0,   0,   0,   0,   5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    };

    static constexpr std::array<int16_t, 64> ROOK_PST = {
         0,   0,   0,   0,   0,   0,   0,   0,
         5,  10,  10,  10,  10,  10,  10,   5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
        -5,   0,   0,   0,   0,   0,   0,  -5,
         0,   0,   0,   5,   5,   0,   0,   0
    };
    static constexpr std::array<int16_t, 64> QUEEN_PST = {
        -20, -10, -10,  -5,  -5, -10, -10, -20,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -10,   0,   5,   5,   5,   5,   0, -10,
         -5,   0,   5,   5,   5,   5,   0,  -5,
          0,   0,   5,   5,   5,   5,   0,  -5,
        -10,   5,   5,   5,   5,   5,   0, -10,
        -10,   0,   5,   0,   0,   0,   0, -10,
        -20, -10, -10,  -5,  -5, -10, -10, -20
    };

    static constexpr std::array<int16_t, 64> KING_MG_PST = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
         20,  20,   0,   0,   0,   0,  20,  20,
         20,  30,  10,   0,   0,  10,  30,  20
    };

    // Endgame king PST - encourages centralization
    static constexpr std::array<int16_t, 64> KING_EG_PST = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10,   0,   0, -10, -20, -30,
        -30, -10,  20,  30,  30,  20, -10, -30,
        -30, -10,  30,  40,  40,  30, -10, -30,
        -30, -10,  30,  40,  40,  30, -10, -30,
        -30, -10,  20,  30,  30,  20, -10, -30,
        -30, -30,   0,   0,   0,   0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
    };

    // Pre-computed masks for ultra-fast lookups
    static constexpr std::array<uint64_t, 8> FILE_MASKS = {
        0x0101010101010101ULL, 0x0202020202020202ULL, 0x0404040404040404ULL,
        0x0808080808080808ULL, 0x1010101010101010ULL, 0x2020202020202020ULL,
        0x4040404040404040ULL, 0x8080808080808080ULL
    };

    static constexpr std::array<uint64_t, 8> RANK_MASKS = {
        0x00000000000000FFULL, 0x000000000000FF00ULL, 0x0000000000FF0000ULL,
        0x00000000FF000000ULL, 0x000000FF00000000ULL, 0x0000FF0000000000ULL,
        0x00FF000000000000ULL, 0xFF00000000000000ULL
    };

    // King safety masks
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

    // Pawn shield masks
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

    static inline bool is_endgame(const board& b) noexcept {
        // Better endgame detection: consider material value
        int total_material = popcount(b.get_white_queens()) + popcount(b.get_black_queens());
        total_material += (popcount(b.get_white_rooks()) + popcount(b.get_black_rooks())) * 2;
        total_material += (popcount(b.get_white_bishops()) + popcount(b.get_black_bishops())) * 2;
        total_material += (popcount(b.get_white_knights()) + popcount(b.get_black_knights())) * 2;

        // Endgame if: no queens and <= 2 minor pieces per side, or low total material
        return total_material <= 6;
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
        // White pieces
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

        // Black pieces (flip vertically with XOR 56)
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

        // Doubled pawns penalty
        for (int file = 0; file < 8; file++) {
            int white_count = popcount(white_pawns & FILE_MASKS[file]);
            int black_count = popcount(black_pawns & FILE_MASKS[file]);

            if (white_count > 1) score -= 10 * (white_count - 1);
            if (black_count > 1) score += 10 * (black_count - 1);

            // Isolated pawns penalty
            uint64_t adjacent_files = 0ULL;
            if (file > 0) adjacent_files |= FILE_MASKS[file - 1];
            if (file < 7) adjacent_files |= FILE_MASKS[file + 1];

            if ((white_pawns & FILE_MASKS[file]) && !(white_pawns & adjacent_files)) {
                score -= 15;
            }
            if ((black_pawns & FILE_MASKS[file]) && !(black_pawns & adjacent_files)) {
                score += 15;
            }
        }

        return score;
    }

    int evaluate_central_control(const board& b) const noexcept {
        int score = 0;

        // Central squares (d4, e4, d5, e5)
        constexpr uint64_t CENTER = 0x0000001818000000ULL;
        // Extended center
        constexpr uint64_t EXTENDED_CENTER = 0x00003C3C3C3C0000ULL;

        uint64_t white_pawns = b.get_white_pawns();
        uint64_t black_pawns = b.get_black_pawns();

        score += popcount(white_pawns & CENTER) * 20;
        score -= popcount(black_pawns & CENTER) * 20;

        score += popcount(white_pawns & EXTENDED_CENTER) * 5;
        score -= popcount(black_pawns & EXTENDED_CENTER) * 5;

        return score;
    }

    int evaluate_piece_bonuses(const board& b) const noexcept {
        int score = 0;

        // Bishop pair bonus
        if (popcount(b.get_white_bishops()) >= 2) score += 50;
        if (popcount(b.get_black_bishops()) >= 2) score -= 50;

        // Rook on 7th rank bonus
        score += popcount(b.get_white_rooks() & RANK_MASKS[6]) * 20;
        score -= popcount(b.get_black_rooks() & RANK_MASKS[1]) * 20;

        return score;
    }

    int evaluate_king_safety(const board& b, bool endgame) const noexcept {
        if (endgame) return 0;

        int score = 0;

        // White king safety
        uint64_t white_king = b.get_white_king();
        if (b.white_castled) {
            score += 40;
        } else {
            score -= 30;
        }

        if (white_king) {
            int king_sq = lsb(white_king);

            // Pawn shield - WHITE KING: use square directly (no flip)
            uint64_t pawn_shield = b.get_white_pawns() & PAWN_SHIELD_MASKS[king_sq];
            int shield_pawns = popcount(pawn_shield);
            score += shield_pawns * 10;

            // Enemy attacks on king zone - WHITE KING: use square directly
            uint64_t enemy_heavy = b.get_black_queens() | b.get_black_rooks();
            if (enemy_heavy) {
                uint64_t king_zone = KING_ZONE_MASKS[king_sq];
                int attackers = popcount(enemy_heavy & king_zone);
                score -= attackers * 15;
            }

            // Open files near king
            int king_file = king_sq % 8;
            for (int f = std::max(0, king_file - 1); f <= std::min(7, king_file + 1); f++) {
                if (!(b.get_white_pawns() & FILE_MASKS[f])) {
                    score -= 20;
                    if (b.get_black_rooks() & FILE_MASKS[f]) {
                        score -= 30;
                    }
                }
            }
        }

        // Black king safety
        uint64_t black_king = b.get_black_king();
        if (b.black_castled) {
            score -= 40;
        } else {
            score += 30;
        }

        if (black_king) {
            int king_sq = lsb(black_king);

            // BLACK KING: MUST flip square with XOR 56 for BOTH shield and zone
            uint64_t pawn_shield = b.get_black_pawns() & PAWN_SHIELD_MASKS[king_sq ^ 56];
            int shield_pawns = popcount(pawn_shield);
            score -= shield_pawns * 10;

            uint64_t enemy_heavy = b.get_white_queens() | b.get_white_rooks();
            if (enemy_heavy) {
                uint64_t king_zone = KING_ZONE_MASKS[king_sq ^ 56];
                int attackers = popcount(enemy_heavy & king_zone);
                score += attackers * 15;
            }

            int king_file = king_sq % 8;
            for (int f = std::max(0, king_file - 1); f <= std::min(7, king_file + 1); f++) {
                if (!(b.get_black_pawns() & FILE_MASKS[f])) {
                    score += 20;
                    if (b.get_white_rooks() & FILE_MASKS[f]) {
                        score += 30;
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
        //score += evaluate_king_safety(b, endgame);
        score += evaluate_central_control(b);

        if (!endgame) {
            score += evaluate_pawn_structure(b);
        }

        return white_to_move ? score : -score;
    }
};

extern Evaluator g_ultra_evaluator;
int evaluate_position(const board& b, bool white_to_move = true) noexcept;

#endif //CHESS_BOT_NEW_ENGINE_HH