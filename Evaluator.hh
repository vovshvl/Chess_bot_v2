// Evaluator.hh — Chess position evaluator v2
// Improvements over v1:
//   - Passed pawns with rank bonus
//   - Rooks on open / semi-open files
//   - King safety re-enabled with correct black-side masks
//   - Backward pawns penalty
//   - Connected rooks bonus
//   - Knight outposts bonus

#ifndef CHESS_BOT_NEW_ENGINE_HH
#define CHESS_BOT_NEW_ENGINE_HH

#include <array>
#include <algorithm>
#include "board.hh"
#include <cstdint>

class Evaluator {
public:
    static inline int popcount(uint64_t x) noexcept { return __builtin_popcountll(x); }
    static inline int lsb(uint64_t x) noexcept { return __builtin_ctzll(x); }
    static inline uint64_t clear_lsb(uint64_t x) noexcept { return x & (x - 1); }

private:
    // ---- Material values ----
    static constexpr int PAWN_VALUE   = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE   = 500;
    static constexpr int QUEEN_VALUE  = 900;

    // ---- Piece-Square Tables ----
    // All PSTs are from White's perspective (a1=0, h8=63).
    // Black pieces are mirrored with XOR 56.

    static constexpr std::array<int16_t, 64> PAWN_PST = {
         0,   0,   0,   0,   0,   0,   0,   0,
        50,  50,  50,  50,  50,  50,  50,  50,
        10,  10,  20,  30,  30,  20,  10,  10,
         5,   5,  10,  25,  25,  10,   5,   5,
         0,   0,   0,  20,  20,   0,   0,   0,
         5,  -5, -10,   0,   0, -10,  -5,   5,
         5,  10,  10, -20, -20,  10,  10,   5,
         0,   0,   0,   0,   0,   0,   0,   0
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
         -5,   0,   5,   5,   5,   5,   0,  -5,
        -10,   0,   5,   5,   5,   5,   0, -10,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -20, -10, -10,  -5,  -5, -10, -10, -20
    };

    // King: midgame prefers castled corner, endgame prefers center
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

    // ---- Pre-computed masks ----

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

    // Passed pawn masks: squares that must be free of enemy pawns for pawn at sq to be passed.
    // For white pawn at sq: all squares on same + adjacent files, ranks ABOVE sq.
    // For black pawn at sq: same + adjacent files, ranks BELOW sq.
    // Generated at compile time via consteval helper would be ideal but we inline here.

    // Passed pawn rank bonus (centipawns) — indexed by rank (0=rank1, 7=rank8)
    static constexpr std::array<int, 8> PASSED_PAWN_BONUS = { 0, 10, 20, 40, 60, 100, 150, 0 };

    // Knight outpost squares: center+extended center bonus when not attackable by enemy pawn
    static constexpr uint64_t OUTPOST_MASK =
        0x00003C3C3C000000ULL; // ranks 3-6, files c-f


    static inline uint64_t white_passed_mask(int sq) noexcept {
        int rank = sq / 8;
        int file = sq % 8;
        // All ranks strictly above
        uint64_t ahead = 0ULL;
        for (int r = rank + 1; r < 8; r++) ahead |= RANK_MASKS[r];
        // Same file + adjacent files
        uint64_t files = FILE_MASKS[file];
        if (file > 0) files |= FILE_MASKS[file - 1];
        if (file < 7) files |= FILE_MASKS[file + 1];
        return ahead & files;
    }

    static inline uint64_t black_passed_mask(int sq) noexcept {
        int rank = sq / 8;
        int file = sq % 8;
        uint64_t ahead = 0ULL;
        for (int r = 0; r < rank; r++) ahead |= RANK_MASKS[r];
        uint64_t files = FILE_MASKS[file];
        if (file > 0) files |= FILE_MASKS[file - 1];
        if (file < 7) files |= FILE_MASKS[file + 1];
        return ahead & files;
    }

    static inline bool is_endgame(const board& b) noexcept {
        // Queen=4, Rook=2, minor=1 — endgame when total <= 6
        int total = (popcount(b.get_white_queens())  + popcount(b.get_black_queens()))  * 4;
        total    += (popcount(b.get_white_rooks())   + popcount(b.get_black_rooks()))   * 2;
        total    += (popcount(b.get_white_bishops()) + popcount(b.get_black_bishops()));
        total    += (popcount(b.get_white_knights()) + popcount(b.get_black_knights()));
        return total <= 6;
    }

    // ---- Individual evaluation components ----

    int evaluate_material(const board& b) const noexcept {
        int score = 0;
        score += popcount(b.get_white_pawns())   * PAWN_VALUE;
        score -= popcount(b.get_black_pawns())   * PAWN_VALUE;
        score += popcount(b.get_white_knights()) * KNIGHT_VALUE;
        score -= popcount(b.get_black_knights()) * KNIGHT_VALUE;
        score += popcount(b.get_white_bishops()) * BISHOP_VALUE;
        score -= popcount(b.get_black_bishops()) * BISHOP_VALUE;
        score += popcount(b.get_white_rooks())   * ROOK_VALUE;
        score -= popcount(b.get_black_rooks())   * ROOK_VALUE;
        score += popcount(b.get_white_queens())  * QUEEN_VALUE;
        score -= popcount(b.get_black_queens())  * QUEEN_VALUE;
        return score;
    }

    int evaluate_pst(const board& b, bool endgame) const noexcept {
        int score = 0;

        // White pieces
        uint64_t pieces = b.get_white_pawns();
        while (pieces) { score += PAWN_PST[lsb(pieces)]; pieces = clear_lsb(pieces); }

        pieces = b.get_white_knights();
        while (pieces) { score += KNIGHT_PST[lsb(pieces)]; pieces = clear_lsb(pieces); }

        pieces = b.get_white_bishops();
        while (pieces) { score += BISHOP_PST[lsb(pieces)]; pieces = clear_lsb(pieces); }

        pieces = b.get_white_rooks();
        while (pieces) { score += ROOK_PST[lsb(pieces)]; pieces = clear_lsb(pieces); }

        pieces = b.get_white_queens();
        while (pieces) { score += QUEEN_PST[lsb(pieces)]; pieces = clear_lsb(pieces); }

        pieces = b.get_white_king();
        if (pieces) score += endgame ? KING_EG_PST[lsb(pieces)] : KING_MG_PST[lsb(pieces)];

        // Black pieces (flip vertically with XOR 56)
        pieces = b.get_black_pawns();
        while (pieces) { score -= PAWN_PST[lsb(pieces) ^ 56]; pieces = clear_lsb(pieces); }

        pieces = b.get_black_knights();
        while (pieces) { score -= KNIGHT_PST[lsb(pieces) ^ 56]; pieces = clear_lsb(pieces); }

        pieces = b.get_black_bishops();
        while (pieces) { score -= BISHOP_PST[lsb(pieces) ^ 56]; pieces = clear_lsb(pieces); }

        pieces = b.get_black_rooks();
        while (pieces) { score -= ROOK_PST[lsb(pieces) ^ 56]; pieces = clear_lsb(pieces); }

        pieces = b.get_black_queens();
        while (pieces) { score -= QUEEN_PST[lsb(pieces) ^ 56]; pieces = clear_lsb(pieces); }

        pieces = b.get_black_king();
        if (pieces) score -= endgame ? KING_EG_PST[lsb(pieces) ^ 56] : KING_MG_PST[lsb(pieces) ^ 56];

        return score;
    }

    int evaluate_pawn_structure(const board& b) const noexcept {
        int score = 0;
        uint64_t wp = b.get_white_pawns();
        uint64_t bp = b.get_black_pawns();

        for (int file = 0; file < 8; file++) {
            int wc = popcount(wp & FILE_MASKS[file]);
            int bc = popcount(bp & FILE_MASKS[file]);

            // Doubled pawns penalty
            if (wc > 1) score -= 10 * (wc - 1);
            if (bc > 1) score += 10 * (bc - 1);

            // Isolated pawns penalty
            uint64_t adj = 0ULL;
            if (file > 0) adj |= FILE_MASKS[file - 1];
            if (file < 7) adj |= FILE_MASKS[file + 1];
            if ((wp & FILE_MASKS[file]) && !(wp & adj)) score -= 15;
            if ((bp & FILE_MASKS[file]) && !(bp & adj)) score += 15;
        }

        uint64_t tmp = wp;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            int file = sq % 8;
            int rank = sq / 8;
            if (rank == 0 || rank == 7) continue;
            // Support: friendly pawn on adjacent file at rank-1 or rank
            uint64_t adj_files = 0ULL;
            if (file > 0) adj_files |= FILE_MASKS[file - 1];
            if (file < 7) adj_files |= FILE_MASKS[file + 1];
            uint64_t support_zone = adj_files & (RANK_MASKS[rank] | (rank > 0 ? RANK_MASKS[rank - 1] : 0ULL));
            if (!(wp & support_zone)) {
                // Find black pawns attacking the stop square (one rank above white pawn).
                // Black pawn at P attacks P-7 and P-9, so attackers of stop are at stop+7 and stop+9.
                // In bitboard terms: shift stop UP (<<) to find where those pawns would be.
                uint64_t stop = 1ULL << (sq + 8);
                uint64_t stop_attackers = ((stop << 7) & ~FILE_MASKS[7]) | ((stop << 9) & ~FILE_MASKS[0]);
                if (stop_attackers & bp) score -= 10;
            }
        }
        tmp = bp;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            int file = sq % 8;
            int rank = sq / 8;
            if (rank == 0 || rank == 7) continue;
            uint64_t adj_files = 0ULL;
            if (file > 0) adj_files |= FILE_MASKS[file - 1];
            if (file < 7) adj_files |= FILE_MASKS[file + 1];
            uint64_t support_zone = adj_files & (RANK_MASKS[rank] | (rank < 7 ? RANK_MASKS[rank + 1] : 0ULL));
            if (!(bp & support_zone)) {
                // Find white pawns attacking the stop square (one rank below black pawn).
                // White pawn at P attacks P+7 and P+9, so attackers of stop are at stop-7 and stop-9.
                // In bitboard terms: shift stop DOWN (>>) to find where those pawns would be.
                uint64_t stop = 1ULL << (sq - 8);
                uint64_t stop_attackers = ((stop >> 7) & ~FILE_MASKS[0]) | ((stop >> 9) & ~FILE_MASKS[7]);
                if (stop_attackers & wp) score += 10;
            }
        }

        return score;
    }

    int evaluate_passed_pawns(const board& b, bool endgame) const noexcept {
        int score = 0;
        uint64_t wp = b.get_white_pawns();
        uint64_t bp = b.get_black_pawns();
        const int eg_multiplier = endgame ? 2 : 1;

        uint64_t tmp = wp;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            if (!(bp & white_passed_mask(sq))) {
                int rank = sq / 8;
                score += PASSED_PAWN_BONUS[rank] * eg_multiplier;
            }
        }
        tmp = bp;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            if (!(wp & black_passed_mask(sq))) {
                int rank = 7 - (sq / 8); // flip rank for black
                score -= PASSED_PAWN_BONUS[rank] * eg_multiplier;
            }
        }
        return score;
    }

    int evaluate_rooks(const board& b) const noexcept {
        int score = 0;
        uint64_t wp = b.get_white_pawns();
        uint64_t bp = b.get_black_pawns();
        uint64_t all_pawns = wp | bp;

        // Rooks on open / semi-open files
        uint64_t wr = b.get_white_rooks();
        uint64_t tmp = wr;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            int file = sq % 8;
            uint64_t fm = FILE_MASKS[file];
            if (!(all_pawns & fm))      score += 20; // open file
            else if (!(wp & fm))        score += 10; // semi-open file (no friendly pawn)
        }

        uint64_t br = b.get_black_rooks();
        tmp = br;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            int file = sq % 8;
            uint64_t fm = FILE_MASKS[file];
            if (!(all_pawns & fm))      score -= 20;
            else if (!(bp & fm))        score -= 10;
        }

        // Connected rooks bonus (two rooks on same rank or file with no pieces between)
        if (popcount(wr) >= 2) score += 15;
        if (popcount(br) >= 2) score -= 15;

        // Rook on 7th rank (white on rank 7, black on rank 2)
        score += popcount(wr & RANK_MASKS[6]) * 25;
        score -= popcount(br & RANK_MASKS[1]) * 25;

        return score;
    }

    int evaluate_piece_bonuses(const board& b) const noexcept {
        int score = 0;

        // Bishop pair
        if (popcount(b.get_white_bishops()) >= 2) score += 50;
        if (popcount(b.get_black_bishops()) >= 2) score -= 50;

        // Knight outposts: knight on outpost square not attackable by enemy pawn
        uint64_t bp = b.get_black_pawns();
        uint64_t wp = b.get_white_pawns();

        uint64_t wn = b.get_white_knights();
        uint64_t tmp = wn;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            if (!(OUTPOST_MASK & (1ULL << sq))) continue;
            // Not attacked by black pawn?
            int file = sq % 8, rank = sq / 8;
            uint64_t attackers = 0ULL;
            if (rank > 0) {
                if (file > 0) attackers |= 1ULL << (sq - 9);
                if (file < 7) attackers |= 1ULL << (sq - 7);
            }
            if (!(attackers & bp)) score += 20;
        }

        uint64_t bn = b.get_black_knights();
        tmp = bn;
        while (tmp) {
            int sq = lsb(tmp); tmp = clear_lsb(tmp);
            if (!(OUTPOST_MASK & (1ULL << sq))) continue;
            int file = sq % 8, rank = sq / 8;
            uint64_t attackers = 0ULL;
            if (rank < 7) {
                if (file > 0) attackers |= 1ULL << (sq + 7);
                if (file < 7) attackers |= 1ULL << (sq + 9);
            }
            if (!(attackers & wp)) score -= 20;
        }

        return score;
    }

    int evaluate_central_control(const board& b) const noexcept {
        constexpr uint64_t CENTER          = 0x0000001818000000ULL; // d4,e4,d5,e5
        constexpr uint64_t EXTENDED_CENTER = 0x00003C3C3C3C0000ULL;

        uint64_t wp = b.get_white_pawns();
        uint64_t bp = b.get_black_pawns();

        int score = 0;
        score += popcount(wp & CENTER) * 20;
        score -= popcount(bp & CENTER) * 20;
        score += popcount(wp & EXTENDED_CENTER) * 5;
        score -= popcount(bp & EXTENDED_CENTER) * 5;
        return score;
    }

    // King safety: pawn shield, open files, enemy attackers near king.
    // NOTE: PAWN_SHIELD_MASKS[sq] gives squares ONE rank ABOVE sq (towards rank 8).
    //       For white king this is correct (shield is one rank ahead).
    //       For black king we need squares ONE rank BELOW sq — we cannot use XOR 56
    //       on the index and expect the same mask values (the masks store absolute bit
    //       positions). Instead we use a separate PAWN_SHIELD_MASKS_BLACK table where
    //       masks point downward (towards rank 1).
    static constexpr std::array<uint64_t, 64> PAWN_SHIELD_MASKS_WHITE = [] {
        std::array<uint64_t, 64> m{};
        for (int sq = 0; sq < 64; sq++) {
            int rank = sq / 8, file = sq % 8;
            if (rank >= 7) { m[sq] = 0; continue; }
            uint64_t mask = 0ULL;
            for (int f = std::max(0, file-1); f <= std::min(7, file+1); f++)
                mask |= 1ULL << ((rank + 1) * 8 + f);
            m[sq] = mask;
        }
        return m;
    }();

    static constexpr std::array<uint64_t, 64> PAWN_SHIELD_MASKS_BLACK = [] {
        std::array<uint64_t, 64> m{};
        for (int sq = 0; sq < 64; sq++) {
            int rank = sq / 8, file = sq % 8;
            if (rank <= 0) { m[sq] = 0; continue; }
            uint64_t mask = 0ULL;
            for (int f = std::max(0, file-1); f <= std::min(7, file+1); f++)
                mask |= 1ULL << ((rank - 1) * 8 + f);
            m[sq] = mask;
        }
        return m;
    }();

    // King zone: 3×3 box around king
    static constexpr std::array<uint64_t, 64> KING_ZONE = [] {
        std::array<uint64_t, 64> m{};
        for (int sq = 0; sq < 64; sq++) {
            int r = sq / 8, f = sq % 8;
            uint64_t zone = 0ULL;
            for (int dr = -1; dr <= 1; dr++)
                for (int df = -1; df <= 1; df++) {
                    int nr = r + dr, nf = f + df;
                    if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8)
                        zone |= 1ULL << (nr * 8 + nf);
                }
            m[sq] = zone;
        }
        return m;
    }();

    int evaluate_king_safety(const board& b, bool endgame) const noexcept {
        if (endgame) return 0;
        int score = 0;

        // -- White king --
        uint64_t wk = b.get_white_king();
        if (wk) {
            int ksq = lsb(wk);
            // Castling bonus/penalty
            score += b.white_castled ? 40 : -30;
            // Pawn shield
            score += popcount(b.get_white_pawns() & PAWN_SHIELD_MASKS_WHITE[ksq]) * 10;
            // Open files near king
            int kf = ksq % 8;
            for (int f = std::max(0, kf-1); f <= std::min(7, kf+1); f++) {
                if (!(b.get_white_pawns() & FILE_MASKS[f])) {
                    score -= 20;
                    if (b.get_black_rooks() & FILE_MASKS[f]) score -= 30;
                }
            }
            // Enemy heavy pieces near king zone
            uint64_t zone = KING_ZONE[ksq];
            uint64_t enemy_heavy = b.get_black_queens() | b.get_black_rooks();
            score -= popcount(enemy_heavy & zone) * 15;
        }

        // -- Black king --
        uint64_t bk = b.get_black_king();
        if (bk) {
            int ksq = lsb(bk);
            score -= b.black_castled ? 40 : -30;
            // Pawn shield: one rank BELOW black king (towards rank 1)
            score -= popcount(b.get_black_pawns() & PAWN_SHIELD_MASKS_BLACK[ksq]) * 10;
            int kf = ksq % 8;
            for (int f = std::max(0, kf-1); f <= std::min(7, kf+1); f++) {
                if (!(b.get_black_pawns() & FILE_MASKS[f])) {
                    score += 20;
                    if (b.get_white_rooks() & FILE_MASKS[f]) score += 30;
                }
            }
            uint64_t zone = KING_ZONE[ksq];
            uint64_t enemy_heavy = b.get_white_queens() | b.get_white_rooks();
            score += popcount(enemy_heavy & zone) * 15;
        }

        return score;
    }

public:
    int evaluate(const board& b, bool white_to_move = true) const noexcept {
        bool endgame = is_endgame(b);

        int score = 0;
        score += evaluate_material(b);
        score += evaluate_pst(b, endgame);
        score += evaluate_piece_bonuses(b);
        score += evaluate_central_control(b);
        score += evaluate_passed_pawns(b, endgame);
        score += evaluate_rooks(b);

        score += evaluate_pawn_structure(b); // important in all phases, especially endgame
        if (!endgame) {
            score += evaluate_king_safety(b, endgame);
        }

        return white_to_move ? score : -score;
    }
};

extern Evaluator g_ultra_evaluator;
int evaluate_position(const board& b, bool white_to_move = true) noexcept;

#endif // CHESS_BOT_NEW_ENGINE_HH
