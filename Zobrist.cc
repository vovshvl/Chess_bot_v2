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
        case 'P': return 1;
        case 'p': return 2;
        case 'N': return 3;
        case 'n': return 4;
        case 'B': return 5;
        case 'b': return 6;
        case 'R': return 7;
        case 'r': return 8;
        case 'Q': return 9;
        case 'q': return 10;
        case 'K': return 11;
        case 'k': return 12;
        default: return -1;

    }
};