//
// Created by Kirill on 8/1/2025.
//

#ifndef BOARD_HH
#include <cstdint>
#include <iostream>
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
};

#endif //BOARD_HH