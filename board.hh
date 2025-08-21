//
// Created by Kirill on 8/1/2025.
//

#ifndef BOARD_HH
#include <cstdint>
#include <iostream>
#include "Move.hh"
#define BOARD_HH

class board {
public:
    // Битборды для каждого типа фигур каждого цвета
    uint64_t white_pawn = 0;
    uint64_t black_pawn = 0;
    uint64_t white_knight = 0;
    uint64_t black_knight = 0;
    uint64_t white_bishop = 0;
    uint64_t black_bishop = 0;
    uint64_t white_rook = 0;
    uint64_t black_rook = 0;
    uint64_t white_queen = 0;
    uint64_t black_queen = 0;
    uint64_t white_king = 0;
    uint64_t black_king = 0;

    // Совмещенные битборды
    uint64_t white_pieces = 0;
    uint64_t black_pieces = 0;
    uint64_t all_pieces = 0;


    void init_board() {
        white_pawn = 0x000000000000FF00ULL;
        black_pawn = 0x00FF000000000000ULL;

        white_rook = 0x0000000000000081ULL;
        black_rook = 0x8100000000000000ULL;

        white_knight = 0x0000000000000042ULL;
        black_knight = 0x4200000000000000ULL;

        white_bishop = 0x0000000000000024ULL;
        black_bishop = 0x2400000000000000ULL;

        white_queen = 0x0000000000000008ULL;
        black_queen = 0x0800000000000000ULL;
        

        white_king = 0x0000000000000010ULL;
        black_king = 0x1000000000000000ULL;

        update_combined_bitboards();
    }
    
    void update_combined_bitboards() {
        white_pieces = white_pawn | white_knight | white_bishop |
                      white_rook | white_queen | white_king;
        black_pieces = black_pawn | black_knight | black_bishop | 
                      black_rook | black_queen | black_king;
        all_pieces = white_pieces | black_pieces;
    }
    
    
    bool is_square_occupied(int square) const {
        return (all_pieces >> square) & 1ULL;
    }
    
    bool is_white_piece(int square) const {
        return (white_pieces >> square) & 1ULL;
    }
    
    bool is_black_piece(int square) const {
        return (black_pieces >> square) & 1ULL;
    }
    
    
    void set_piece(int square, char piece) {
        uint64_t bit = 1ULL << square;
        
        // Сначала очищаем поле
        clear_square(square);

        switch(piece) {
            case 'P': white_pawn |= bit; break;
            case 'p': black_pawn |= bit; break;
            case 'N': white_knight |= bit; break;
            case 'n': black_knight |= bit; break;
            case 'B': white_bishop |= bit; break;
            case 'b': black_bishop |= bit; break;
            case 'R': white_rook |= bit; break;
            case 'r': black_rook |= bit; break;
            case 'Q': white_queen |= bit; break;
            case 'q': black_queen |= bit; break;
            case 'K': white_king |= bit; break;
            case 'k': black_king |= bit; break;
        }
        
        update_combined_bitboards();
    }
    
    void clear_square(int square) {
        uint64_t clear_bit = ~(1ULL << square);
        white_pawn &= clear_bit;
        black_pawn &= clear_bit;
        white_knight &= clear_bit;
        black_knight &= clear_bit;
        white_bishop &= clear_bit;
        black_bishop &= clear_bit;
        white_rook &= clear_bit;
        black_rook &= clear_bit;
        white_queen &= clear_bit;
        black_queen &= clear_bit;
        white_king &= clear_bit;
        black_king &= clear_bit;
        
        update_combined_bitboards();
    }

    char get_piece_at_square(int square) const {
        uint64_t bit = 1ULL << square;
        
        if (white_pawn & bit) return 'P';
        if (black_pawn & bit) return 'p';
        if (white_knight & bit) return 'N';
        if (black_knight & bit) return 'n';
        if (white_bishop & bit) return 'B';
        if (black_bishop & bit) return 'b';
        if (white_rook & bit) return 'R';
        if (black_rook & bit) return 'r';
        if (white_queen & bit) return 'Q';
        if (black_queen & bit) return 'q';
        if (white_king & bit) return 'K';
        if (black_king & bit) return 'k';

        return '.'; // пустое поле
    }
    
    // Функция для печати доски
    void print_board() const {
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
        
        // Печатаем доску с 8-й горизонтали (индекс 56-63) до 1-й (индекс 0-7)
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << (rank + 1) << " |";
            
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                char piece = get_piece_at_square(square);
                std::cout << " " << piece << " |";
            }
            
            std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
        }
        
        std::cout << "    a   b   c   d   e   f   g   h\n\n";
    }
    void reset_board() {
        // Очищаем все битборды
        white_pawn = black_pawn = 0;
        white_knight = black_knight = 0;
        white_bishop = black_bishop = 0;
        white_rook = black_rook = 0;
        white_queen = black_queen = 0;
        white_king = black_king = 0;
        white_pieces = black_pieces = all_pieces = 0;
        

        init_board();
    }
    void execute_move(const Move& move) {
        int from = move.get_from_square();
        int to = move.get_to_square();

        char moving_piece = get_piece_at_square(from);
        char captured_piece = get_piece_at_square(to);

        // Перемещаем фигуру
        clear_square(from);
        set_piece(to, moving_piece);
    }
void reverse_move(const Move& move) {
    int from = move.get_from_square();
    int to = move.get_to_square();
    char moving_piece = get_piece_at_square(to);
    char captured_piece = move.get_captured_piece();

    clear_square(to);
    set_piece(from, moving_piece);

    if (captured_piece != '.') {
        set_piece(to, captured_piece);
    }
}

    uint64_t get_white_pawns() const { return white_pawn; }
    uint64_t get_black_pawns() const { return black_pawn; }
    uint64_t get_white_knights() const { return white_knight; }
    uint64_t get_black_knights() const { return black_knight; }
    uint64_t get_white_bishops() const { return white_bishop; }
    uint64_t get_black_bishops() const { return black_bishop; }
    uint64_t get_white_rooks() const { return white_rook; }
    uint64_t get_black_rooks() const { return black_rook; }
    uint64_t get_white_queens() const { return white_queen; }
    uint64_t get_black_queens() const { return black_queen; }
    uint64_t get_white_king() const { return white_king; }
    uint64_t get_black_king() const { return black_king; }
    uint64_t get_white_pieces() const { return white_pieces; }
    uint64_t get_black_pieces() const { return black_pieces; }
    uint64_t get_all_pieces() const { return all_pieces; }
    uint64_t get_occupancy() const { return white_pieces | black_pieces; }
    uint64_t get_pieces_by_value( const int value) const {
        switch (value) {
                case 1: return get_white_pawns();
                case -1: return get_black_pawns();
                case 2: return get_white_knights();
                case -2: return get_black_knights();
                case 3: return get_white_bishops();
                case -3: return get_black_bishops();
                case 4: return get_white_rooks();
                case -4: return get_black_rooks();
                case 5: return get_white_queens();
                case -5: return get_black_queens();
                case 6: return get_white_king();
                case -6: return get_black_king();
                default: return 0;

        }
    };
    uint8_t get_rank(uint64_t board, int rank) {
        return (board >> (rank*8)) & 0xFF;
    }

    uint8_t get_file(uint64_t board, int file) {
        uint8_t f = 0;
        for (int r = 0; r < 8; ++r)
            if (board & (1ULL << (r*8 + file))) f |= (1 << r);
        return f;
    }
    // Returns a bitboard with only the piece at the given square (0 if no piece)
    uint64_t get_piece_bitboard_at_square(int square) const {
        uint64_t mask = 1ULL << square;

        if (white_pawn & mask)   return white_pawn & mask;
        if (black_pawn & mask)   return black_pawn & mask;
        if (white_knight & mask) return white_knight & mask;
        if (black_knight & mask) return black_knight & mask;
        if (white_bishop & mask) return white_bishop & mask;
        if (black_bishop & mask) return black_bishop & mask;
        if (white_rook & mask)   return white_rook & mask;
        if (black_rook & mask)   return black_rook & mask;
        if (white_queen & mask)  return white_queen & mask;
        if (black_queen & mask)  return black_queen & mask;
        if (white_king & mask)   return white_king & mask;
        if (black_king & mask)   return black_king & mask;

        return 0ULL;
    }
    // Reverse the bits of an 8-bit number, helper for moves
    static uint8_t reverse8(uint8_t b) {
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
        return b;
    }
// Convert 8-bit rank attack mask back to 64-bit bitboard
    static uint64_t rank_to_bitboard(uint8_t rank_mask, int rank) {
        return ((uint64_t)rank_mask) << (rank * 8);
    }

// Convert 8-bit file attack mask back to 64-bit bitboard
    uint64_t file_to_bitboard(uint8_t file_bits, int file) {
        uint64_t bb = 0ULL;
        for (int r = 0; r < 8; r++) {
            if (file_bits & (1 << r)) {
                // bit r of file_bits corresponds to rank r
                bb |= 1ULL << (r*8 + file);
            }
        }
        return bb;
    }



};

#endif //BOARD_HH