//
// Created by Kirill on 8/1/2025.
//

#ifndef BOARD_HH
#include <cstdint>
#include <iostream>
#include <vector>
#include "Move.hh"
#define BOARD_HH
struct UndoInfo {
    char captured_piece;
    char moving_piece;
    bool king_castle_white, queen_castle_white;
    bool king_castle_black, queen_castle_black;
};
struct Move {
    int from;
    int to;
    char promotion = '\0'; // '0' if no promotion, otherwise 'q','r','b','n'
    //bool castling; //ev. redo or better integrate

    bool operator<(const Move& other) const {
        if (from != other.from) return from < other.from;
        if (to != other.to) return to < other.to;
        return promotion < other.promotion;
    }

    bool operator==(const Move& other) const {
        return from == other.from && to == other.to && promotion == other.promotion;
    }
};

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

    //Castling flags, when rook or king moves set to false
    bool king_castle_black = true;
    bool king_castle_white = true;
    bool queen_castle_black = true;
    bool queen_castle_white = true;

    std::vector<UndoInfo> history_info; //For fast do and undo
    std::vector<Move> history_moves;

    bool white_to_move = true; //For minmax


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
    /*
    void execute_move_on_bitboard(const Move& m){
        int from = m.from;
        int to = m.to;
        char promotion = m.promotion;
        char moving_piece = get_piece_at_square(from);
        char captured_piece = get_piece_at_square(to);


        //Castling, so the rook moves too
        if(moving_piece =='K' and from == 4 and (to == 6 or to==2)){
            if(to==6){
                clear_square(7);
                set_piece(5, 'R');
            }
            if(to==2){
                clear_square(0);
                set_piece(3, 'R');
            }
        }
        else if (moving_piece =='k' and from == 60 and (to == 58 or to==62)){
            if(to==62){
                clear_square(63);
                set_piece(61, 'r');
            }
            if(to==58){
                clear_square(56);
                set_piece(59, 'r');
            }
        }
        if(moving_piece == 'K'){
            king_castle_white = false;
            queen_castle_white = false;
        }
        else if(moving_piece == 'k'){
            king_castle_black = false;
            queen_castle_black = false;
        }
        else if(moving_piece == 'R'){
            if(from == 0) queen_castle_white = false;
            if(from == 7) king_castle_white = false;
        }
        else if(moving_piece == 'r'){
            if(from == 56) queen_castle_black = false;
            if(from == 63) king_castle_black = false;
        }

        if(captured_piece == 'R'){
            if(to == 0) queen_castle_white = false;
            if(to == 7) king_castle_white = false;
        }
        else if(captured_piece == 'r'){
            if(to == 56) queen_castle_black = false;
            if(to == 63) king_castle_black = false;
        }
        clear_square(from);
        if(promotion != '\0'){
            // Pawn promotion
            set_piece(to, (isupper(moving_piece) ? toupper(promotion) : tolower(promotion)));
        } else {
            // Normal move
            set_piece(to, moving_piece);
        }
    }
    void execute_move(const Move& m) {

        UndoInfo undo;
        undo.captured_piece = get_piece_at_square(m.to);
        undo.king_castle_white = king_castle_white;
        undo.king_castle_black = king_castle_black;
        undo.queen_castle_white = queen_castle_white;
        undo.queen_castle_black = queen_castle_black;

        execute_move_on_bitboard(m);
        history_info.push_back(undo);
        history_moves.push_back(m);
    }
    void reverse_move_on_bitboard(const Move& m, const UndoInfo& undo){
        int from = m.from;
        int to = m.to;
        char promotion = m.promotion;
        char moving_piece = get_piece_at_square(to);

        clear_square(to);
        set_piece(from, moving_piece);

        if (undo.captured_piece != '.') {
            set_piece(to, undo.captured_piece);
        }

        if (moving_piece == 'K' && (m.to == 6 || m.to == 2)) {
            if (m.to == 6) { clear_square(5); set_piece(7,'R'); }
            if (m.to == 2) { clear_square(3); set_piece(0,'R'); }
        }
        if (moving_piece == 'k' && (m.to == 62 || m.to == 58)) {
            if (m.to == 62) { clear_square(61); set_piece(63,'r'); }
            if (m.to == 58) { clear_square(59); set_piece(56,'r'); }
        }
    }
    void reverse_move(const Move& m) {
        if (history_moves.empty()) return;
        UndoInfo undo = history_info.back();
        history_info.pop_back();

        king_castle_white  = undo.king_castle_white;
        queen_castle_white = undo.queen_castle_white;
        king_castle_black  = undo.king_castle_black;
        queen_castle_black = undo.queen_castle_black;

        reverse_move_on_bitboard(m, undo);
}
     */
    // set_piece and then update_combined_bitboards separately for from and to was too slow
    inline void update_combined_bitboards_incremental(uint64_t from_mask, uint64_t to_mask,
                                                      char piece, bool is_capture, char captured_piece) {
        // Remove captured piece first
        if (is_capture) {
            switch (captured_piece) {
                case 'P': white_pawn &= ~to_mask; break;
                case 'p': black_pawn &= ~to_mask; break;
                case 'N': white_knight &= ~to_mask; break;
                case 'n': black_knight &= ~to_mask; break;
                case 'B': white_bishop &= ~to_mask; break;
                case 'b': black_bishop &= ~to_mask; break;
                case 'R': white_rook &= ~to_mask; break;
                case 'r': black_rook &= ~to_mask; break;
                case 'Q': white_queen &= ~to_mask; break;
                case 'q': black_queen &= ~to_mask; break;
                case 'K': white_king &= ~to_mask; break;
                case 'k': black_king &= ~to_mask; break;
            }
        }

        // Move the piece
        switch (piece) {
            case 'P': white_pawn &= ~from_mask; white_pawn |= to_mask; break;
            case 'p': black_pawn &= ~from_mask; black_pawn |= to_mask; break;
            case 'N': white_knight &= ~from_mask; white_knight |= to_mask; break;
            case 'n': black_knight &= ~from_mask; black_knight |= to_mask; break;
            case 'B': white_bishop &= ~from_mask; white_bishop |= to_mask; break;
            case 'b': black_bishop &= ~from_mask; black_bishop |= to_mask; break;
            case 'R': white_rook &= ~from_mask; white_rook |= to_mask; break;
            case 'r': black_rook &= ~from_mask; black_rook |= to_mask; break;
            case 'Q': white_queen &= ~from_mask; white_queen |= to_mask; break;
            case 'q': black_queen &= ~from_mask; black_queen |= to_mask; break;
            case 'K': white_king &= ~from_mask; white_king |= to_mask; break;
            case 'k': black_king &= ~from_mask; black_king |= to_mask; break;
        }

        // Recompute combined bitboards
        white_pieces = white_pawn | white_knight | white_bishop | white_rook | white_queen | white_king;
        black_pieces = black_pawn | black_knight | black_bishop | black_rook | black_queen | black_king;
        all_pieces   = white_pieces | black_pieces;
    }



    void execute_move_on_bitboard(const Move& m) {
        int from = m.from;
        int to   = m.to;
        char promotion = m.promotion;

        char moving_piece   = get_piece_at_square(from);
        char captured_piece = get_piece_at_square(to);

        if(m.from==28 && m.to==35){
            //std::cout<<"from: "<<from<<"to: "<<to<<"cap_piece: "<<captured_piece;
        }


        uint64_t from_mask = 1ULL << from;
        uint64_t to_mask   = 1ULL << to;

        // --- Handle castling ---
        if (moving_piece == 'K' && from == 4) {
            if (to == 6) { // White king-side
                white_rook &= ~(1ULL << 7);
                white_rook |= 1ULL << 5;
                king_castle_white = queen_castle_white = false;
            } else if (to == 2) { // White queen-side
                white_rook &= ~(1ULL << 0);
                white_rook |= 1ULL << 3;
                king_castle_white = queen_castle_white = false;
            }
        } else if (moving_piece == 'k' && from == 60) {
            if (to == 62) { // Black king-side
                black_rook &= ~(1ULL << 63);
                black_rook |= 1ULL << 61;
                king_castle_black = queen_castle_black = false;
            } else if (to == 58) { // Black queen-side
                black_rook &= ~(1ULL << 56);
                black_rook |= 1ULL << 59;
                king_castle_black = queen_castle_black = false;
            }
        }

        // --- Update castling rights if king or rook moves ---
        if (moving_piece == 'K') king_castle_white = queen_castle_white = false;
        if (moving_piece == 'k') king_castle_black = queen_castle_black = false;
        if (moving_piece == 'R') { if (from == 0) queen_castle_white = false; if (from == 7) king_castle_white = false; }
        if (moving_piece == 'r') { if (from == 56) queen_castle_black = false; if (from == 63) king_castle_black = false; }

        // --- Helper lambdas ---
        auto remove_piece_at = [&](char piece, uint64_t mask){
            switch(piece){
                case 'P': white_pawn &= ~mask; break;
                case 'N': white_knight &= ~mask; break;
                case 'B': white_bishop &= ~mask; break;
                case 'R': white_rook &= ~mask; break;
                case 'Q': white_queen &= ~mask; break;
                case 'K': white_king &= ~mask; break;
                case 'p': black_pawn &= ~mask; break;
                case 'n': black_knight &= ~mask; break;
                case 'b': black_bishop &= ~mask; break;
                case 'r': black_rook &= ~mask; break;
                case 'q': black_queen &= ~mask; break;
                case 'k': black_king &= ~mask; break;
            }
        };

        auto move_piece = [&](char piece, uint64_t from_mask, uint64_t to_mask){
            switch(piece){
                case 'P': white_pawn   = (white_pawn & ~from_mask)   | to_mask; break;
                case 'N': white_knight = (white_knight & ~from_mask) | to_mask; break;
                case 'B': white_bishop = (white_bishop & ~from_mask) | to_mask; break;
                case 'R': white_rook   = (white_rook & ~from_mask)   | to_mask; break;
                case 'Q': white_queen  = (white_queen & ~from_mask)  | to_mask; break;
                case 'K': white_king   = (white_king & ~from_mask)   | to_mask; break;
                case 'p': black_pawn   = (black_pawn & ~from_mask)   | to_mask; break;
                case 'n': black_knight = (black_knight & ~from_mask) | to_mask; break;
                case 'b': black_bishop = (black_bishop & ~from_mask) | to_mask; break;
                case 'r': black_rook   = (black_rook & ~from_mask)   | to_mask; break;
                case 'q': black_queen  = (black_queen & ~from_mask)  | to_mask; break;
                case 'k': black_king   = (black_king & ~from_mask)   | to_mask; break;
            }
        };

        // --- Remove captured piece (if any) ---
        if(captured_piece != '.') remove_piece_at(captured_piece, to_mask);

        // --- Handle promotion separately ---
        if(promotion != '\0') {
            remove_piece_at(moving_piece, from_mask);
            char promo_piece = isupper(moving_piece) ? toupper(promotion) : tolower(promotion);
            move_piece(promo_piece, 0, to_mask);
        } else {
            // Normal move
            move_piece(moving_piece, from_mask, to_mask);
        }

        // --- Recompute combined bitboards ---
        white_pieces = white_pawn | white_knight | white_bishop | white_rook | white_queen | white_king;
        black_pieces = black_pawn | black_knight | black_bishop | black_rook | black_queen | black_king;
        all_pieces   = white_pieces | black_pieces;
    }

    void execute_move(const Move& m) {
        // Prepare undo info
        UndoInfo undo;
        undo.captured_piece   = get_piece_at_square(m.to);
        undo.moving_piece     = get_piece_at_square(m.from);
        undo.king_castle_white  = king_castle_white;
        undo.queen_castle_white = queen_castle_white;
        undo.king_castle_black  = king_castle_black;
        undo.queen_castle_black = queen_castle_black;

        execute_move_on_bitboard(m);

        history_moves.push_back(m);
        history_info.push_back(undo);

        white_to_move = !white_to_move;


        update_combined_bitboards();
    }

// Reverse move efficiently
    void reverse_move_on_bitboard(const Move& m, const UndoInfo& undo) {
        int from = m.from;
        int to   = m.to;
        char promotion = m.promotion;

        uint64_t from_mask = 1ULL << from;
        uint64_t to_mask   = 1ULL << to;

        char moving_piece = (promotion != '\0') ? (isupper(undo.moving_piece) ? 'P' : 'p')
                                                : undo.moving_piece;

        // Undo castling rooks
        if (moving_piece == 'K') {
            if (to == 6) { white_rook &= ~(1ULL << 5); white_rook |= 1ULL << 7; }
            if (to == 2) { white_rook &= ~(1ULL << 3); white_rook |= 1ULL << 0; }
        } else if (moving_piece == 'k') {
            if (to == 62) { black_rook &= ~(1ULL << 61); black_rook |= 1ULL << 63; }
            if (to == 58) { black_rook &= ~(1ULL << 59); black_rook |= 1ULL << 56; }
        }

        // Undo the move
        if (promotion != '\0') {
            // Remove promoted piece and restore pawn
            switch (promotion) {
                case 'Q': white_queen  &= ~to_mask; break;
                case 'R': white_rook   &= ~to_mask; break;
                case 'B': white_bishop &= ~to_mask; break;
                case 'N': white_knight &= ~to_mask; break;
                case 'q': black_queen  &= ~to_mask; break;
                case 'r': black_rook   &= ~to_mask; break;
                case 'b': black_bishop &= ~to_mask; break;
                case 'n': black_knight &= ~to_mask; break;
            }

            if (isupper(undo.moving_piece)) white_pawn |= from_mask;
            else black_pawn |= from_mask;

        } else {
            // Normal move: move piece back, no capture
            update_combined_bitboards_incremental(to_mask, from_mask, moving_piece, false, '.');
        }

        // Restore captured piece if any
        if (undo.captured_piece != '.') {
            uint64_t cap_mask = 1ULL << to;
            switch (undo.captured_piece) {
                case 'P': white_pawn   |= cap_mask; break;
                case 'N': white_knight |= cap_mask; break;
                case 'B': white_bishop |= cap_mask; break;
                case 'R': white_rook   |= cap_mask; break;
                case 'Q': white_queen  |= cap_mask; break;
                case 'K': white_king   |= cap_mask; break;
                case 'p': black_pawn   |= cap_mask; break;
                case 'n': black_knight |= cap_mask; break;
                case 'b': black_bishop |= cap_mask; break;
                case 'r': black_rook   |= cap_mask; break;
                case 'q': black_queen  |= cap_mask; break;
                case 'k': black_king   |= cap_mask; break;
            }
        }

        // Restore castling rights
        king_castle_white  = undo.king_castle_white;
        queen_castle_white = undo.queen_castle_white;
        king_castle_black  = undo.king_castle_black;
        queen_castle_black = undo.queen_castle_black;

        // Update combined bitboards
        update_combined_bitboards();
    }


    void reverse_move(const Move& m) {
        if (history_moves.empty()) return;

        // Pop the last move and undo info
        Move last_move = history_moves.back();
        history_moves.pop_back();

        UndoInfo undo = history_info.back();
        history_info.pop_back();

        white_to_move = !white_to_move;

        // Undo the move on the bitboards
        reverse_move_on_bitboard(last_move, undo);
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
    bool get_king_castle_black() const {return king_castle_black;}
    bool get_king_castle_white() const {return king_castle_white;}
    bool get_queen_castle_black() const {return queen_castle_black;}
    bool get_queen_castle_white() const {return queen_castle_white;}
    static uint8_t get_rank(uint64_t board, int rank) {
        return (board >> (rank*8)) & 0xFF;
    }

    static uint8_t get_file(uint64_t board, int file) {
        uint8_t f = 0;
        for (int r = 0; r < 8; ++r)
            if (board & (1ULL << (r*8 + file))) f |= (1 << r);
        return f;
    }

    bool get_white_to_move(){
        return white_to_move;
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
    static uint64_t file_to_bitboard(uint8_t file_bits, int file) {
        uint64_t bb = 0ULL;
        for (int r = 0; r < 8; r++) {
            if (file_bits & (1 << r)) {
                // bit r of file_bits corresponds to rank r
                bb |= 1ULL << (r*8 + file);
            }
        }
        return bb;
    }

    static uint64_t squares_between(int sq1, int sq2) {
        uint64_t mask = 0ULL;

        int r1 = sq1 / 8, f1 = sq1 % 8;
        int r2 = sq2 / 8, f2 = sq2 % 8;

        int dr = (r2 - r1) ? ((r2 - r1) / abs(r2 - r1)) : 0;
        int df = (f2 - f1) ? ((f2 - f1) / abs(f2 - f1)) : 0;

        // Not on same rank/file/diagonal → no squares between
        if (!((dr == 0 && df != 0) || (dr != 0 && df == 0) || (abs(dr) == abs(df)))) {
            return 0ULL;
        }

        int r = r1 + dr;
        int f = f1 + df;
        while (r != r2 || f != f2) {
            mask |= 1ULL << (r * 8 + f);
            r += dr;
            f += df;
        }

        return mask;
    }


};

#endif //BOARD_HH