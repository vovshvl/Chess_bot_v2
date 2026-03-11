#include "Zobrist.hh"
#include <random>


std::array<std::array<uint64_t, 64>, 12> Zobrist::piece_square;
std::array<uint64_t, 16> Zobrist::castling_rights;
std::array<uint64_t, 8>  Zobrist::en_passant_file;
uint64_t Zobrist::side_to_move;


void Zobrist::init(){
    std::mt19937_64 rng(2025);

    for (int piece = 0; piece < 12; ++piece)
        for (int sq = 0; sq < 64; ++sq)
            piece_square[piece][sq] = rng();

    side_to_move = rng();

    for (int i = 0; i < 16; ++i) castling_rights[i] = rng();
    for (int i = 0; i < 8; ++i) en_passant_file[i] = rng();
}

int Zobrist::castling_rights_index_zobrist(bool wk, bool wq, bool bk, bool bq) {
    return (wk ? 1 : 0) | (wq ? 2 : 0) | (bk ? 4 : 0) | (bq ? 8 : 0);
}

int Zobrist::get_value_by_piece_zobrist( const char piece) {
    switch (piece) {
        case 'P': return 0;
        case 'p': return 1;
        case 'N': return 2;
        case 'n': return 3;
        case 'B': return 4;
        case 'b': return 5;
        case 'R': return 6;
        case 'r': return 7;
        case 'Q': return 8;
        case 'q': return 9;
        case 'K': return 10;
        case 'k': return 11;
        default: return -1;

    }
};