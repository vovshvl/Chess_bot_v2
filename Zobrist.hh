// zobrist.h
#pragma once
#include <cstdint>
#include <array>

class Zobrist {
public:
    static std::array<std::array<uint64_t, 64>, 12> piece_square; // [piece][square]
    static std::array<uint64_t, 16> castling_rights;
    static std::array<uint64_t, 8> en_passant_file;  // file a-h
    static uint64_t side_to_move;

    static void init();
    int castling_rights_index_zobrist(bool wk, bool wq, bool bk, bool bq);
    static int get_value_by_piece_zobrist(const char piece);
};
