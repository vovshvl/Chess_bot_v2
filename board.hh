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
    char captured_piece = '.';
    char moving_piece='.';
    bool king_castle_white, queen_castle_white = true;
    bool king_castle_black, queen_castle_black = true;
    bool white_castled = false;
    bool black_castled = false;
    int en_passant_square = -1;
};
struct Move {
    int from;
    int to;
    char promotion = '\0'; // '0' if no promotion, otherwise 'q','r','b','n'

    //bool castling; //ev. redo or better integrate
    //UndoInfo undo_info = {'.', '.', true, true, true, true};

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

    //Castling flags to determine castling
    bool white_castled = false;
    bool black_castled = false;

    std::vector<UndoInfo> history_info; //For fast do and undo
    std::vector<Move> history_moves;

    bool white_to_move = true; //For minmax
    int en_passant_sq = -1;


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

    void set_piece_fast(int square, char piece){//without updating bitboards to be fast
        uint64_t bit = 1ULL << square;
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

    inline void clear_square_fast(int square) { //faster version without updating bitboards
        uint64_t bit = ~(1ULL << square);
        white_pawn   &= bit;
        black_pawn   &= bit;
        white_knight &= bit;
        black_knight &= bit;
        white_bishop &= bit;
        black_bishop &= bit;
        white_rook   &= bit;
        black_rook   &= bit;
        white_queen  &= bit;
        black_queen  &= bit;
        white_king   &= bit;
        black_king   &= bit;
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

    void print_different_board(const uint64_t& bb) const {
        std::cout << "\n  +---+---+---+---+---+---+---+---+\n";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << (rank + 1) << " |";
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                if ((bb >> square) & 1) {
                    std::cout << " 1 |";
                } else {
                    std::cout << " 0 |";
                }
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

    void remove_piece_at(char piece, uint64_t mask) {
        switch (piece) {
            case 'P': white_pawn   &= ~mask; break;
            case 'N': white_knight &= ~mask; break;
            case 'B': white_bishop &= ~mask; break;
            case 'R': white_rook   &= ~mask; break;
            case 'Q': white_queen  &= ~mask; break;
            case 'K': white_king   &= ~mask; break;

            case 'p': black_pawn   &= ~mask; break;
            case 'n': black_knight &= ~mask; break;
            case 'b': black_bishop &= ~mask; break;
            case 'r': black_rook   &= ~mask; break;
            case 'q': black_queen  &= ~mask; break;
            case 'k': black_king   &= ~mask; break;
        }
    }

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

        uint64_t& moving_piece_bitboard = get_piece_bitboard_ref(moving_piece);


        uint64_t from_mask = 1ULL << from;
        uint64_t to_mask   = 1ULL << to;

        // --- Handle castling ---
        if (moving_piece == 'K' && from == 4) {
            if (to == 6) { // White king-side
                white_rook &= ~(1ULL << 7);
                white_rook |= 1ULL << 5;
                king_castle_white = queen_castle_white = false;
                white_castled = true;
            } else if (to == 2) { // White queen-side
                white_rook &= ~(1ULL << 0);
                white_rook |= 1ULL << 3;
                king_castle_white = queen_castle_white = false;
                white_castled = true;
            }
        } else if (moving_piece == 'k' && from == 60) {
            if (to == 62) { // Black king-side
                black_rook &= ~(1ULL << 63);
                black_rook |= 1ULL << 61;
                king_castle_black = queen_castle_black = false;
                black_castled = true;
            } else if (to == 58) { // Black queen-side
                black_rook &= ~(1ULL << 56);
                black_rook |= 1ULL << 59;
                king_castle_black = queen_castle_black = false;
                black_castled = true;
            }
        }

        // --- Update castling rights if king or rook moves ---
        if (moving_piece == 'K') king_castle_white = queen_castle_white = false;
        if (moving_piece == 'k') king_castle_black = queen_castle_black = false;
        if (moving_piece == 'R') { if (from == 0) queen_castle_white = false; if (from == 7) king_castle_white = false; }
        if (moving_piece == 'r') { if (from == 56) queen_castle_black = false; if (from == 63) king_castle_black = false; }

        //Remove captured piece
        if(captured_piece != '.') {
            uint64_t& captured_piece_bitboard = get_piece_bitboard_ref(captured_piece);
            captured_piece_bitboard &= ~to_mask;

        }

        moving_piece_bitboard &= ~from_mask; //remove piece from from square

        // --- Handle promotion separately ---
        if(promotion != '\0') {
            char promo_piece = isupper(moving_piece) ? toupper(promotion) : tolower(promotion);
            uint64_t& promotion_bitboard = get_piece_bitboard_ref(promo_piece);
            promotion_bitboard |= to_mask;

        } else {
            // Normal move
            moving_piece_bitboard |= to_mask; //adds piece to to square
        }

        // --- Handle en passant ---

        /*
        en_passant_sq = -1;
        if (moving_piece == 'P' && from/8 == 1 && to/8 == 3) {
            en_passant_sq = 2*8 + (from % 8);  // white pawn jumped, ep on rank 3
        } else if (moving_piece == 'p' && from/8 == 6 && to/8 == 4) {
            en_passant_sq = 4*8 + (from % 8);  // black pawn jumped, ep on rank 6
        }
        */

        en_passant_sq = -1;
        int file = to % 8;

        if (moving_piece == 'P' && from/8 == 1 && to/8 == 3) {
            if ((file > 0 && (black_pawn & (1ULL << (to - 1)))) ||
                (file < 7 && (black_pawn & (1ULL << (to + 1))))) {
                en_passant_sq = to;
            }
        }
        else if (moving_piece == 'p' && from/8 == 6 && to/8 == 4) {
            if ((file > 0 && (white_pawn & (1ULL << (to - 1)))) ||
                (file < 7 && (white_pawn & (1ULL << (to + 1))))) {
                en_passant_sq = to;
            }
        }

        if (moving_piece == 'P' && captured_piece == '.' && abs(to % 8 - from % 8) == 1 && to/8 == 5) {
            uint64_t captured_pawn_bb = 1ULL << (to - 8);
            black_pawn &= ~captured_pawn_bb;
        }
        else if (moving_piece == 'p' && captured_piece == '.' && abs(to % 8 - from % 8) == 1 && to/8 == 2) {
            uint64_t captured_pawn_bb = 1ULL << (to + 8);
            white_pawn &= ~captured_pawn_bb;
        }

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
        undo.white_castled = white_castled;
        undo.black_castled = black_castled;
        if(en_passant_sq != -1){
            undo.en_passant_square = en_passant_sq;
            undo.captured_piece   = get_piece_at_square(en_passant_sq);
        }

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

        char moving_piece = (promotion != '\0') ? (isupper(undo.moving_piece) ? 'P' : 'p') : undo.moving_piece;
        uint64_t& moving_piece_bitboard = get_piece_bitboard_ref(moving_piece);



        // Undo castling rooks
        if (moving_piece == 'K' && from == 4) {
            if (to == 6) { white_rook &= ~(1ULL << 5); white_rook |= 1ULL << 7;}
            if (to == 2) { white_rook &= ~(1ULL << 3); white_rook |= 1ULL << 0;}
        } else if (moving_piece == 'k' && from == 60) {
            if (to == 62) { black_rook &= ~(1ULL << 61); black_rook |= 1ULL << 63;}
            if (to == 58) { black_rook &= ~(1ULL << 59); black_rook |= 1ULL << 56;}
        }

        moving_piece_bitboard &= ~to_mask; //remove piece from to

        // Undo the move
        if (promotion != '\0') {
            // Remove promoted piece
            char promo_piece = isupper(undo.moving_piece) ? toupper(promotion) : tolower(promotion);
            uint64_t& promo_bitboard = get_piece_bitboard_ref(promo_piece);
            promo_bitboard &= ~to_mask;

            // Restore pawn
            uint64_t& pawn_bitboard = get_piece_bitboard_ref(undo.moving_piece);
            pawn_bitboard |= from_mask;
        } else {
            uint64_t& piece_bitboard = get_piece_bitboard_ref(undo.moving_piece);
            piece_bitboard &= ~to_mask;
            piece_bitboard |= from_mask;
        }

        // Restore captured piece if any
        if (undo.captured_piece != '.') {
            uint64_t cap_mask;
            if(undo.en_passant_square != -1){
                cap_mask = 1ULL << (m.to + (white_to_move ? -8 : 8));
            }
            else{
                cap_mask = 1ULL << to;
            }

            uint64_t& captured_bitboard = get_piece_bitboard_ref(undo.captured_piece);
            captured_bitboard |= cap_mask;
        }

        // Restore castling rights
        king_castle_white  = undo.king_castle_white;
        queen_castle_white = undo.queen_castle_white;
        king_castle_black  = undo.king_castle_black;
        queen_castle_black = undo.queen_castle_black;
        white_castled = undo.white_castled;
        black_castled = undo.black_castled;

        en_passant_sq = undo.en_passant_square;
    }


    void reverse_move(const Move& m) {
        if (history_moves.empty()) return;

        // Pop the last move and undo info
        Move last_move = history_moves.back();
        history_moves.pop_back();

        UndoInfo undo = history_info.back();
        history_info.pop_back();



        // Undo the move on the bitboards
        reverse_move_on_bitboard(last_move, undo);



        white_to_move = !white_to_move;
        update_combined_bitboards();
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
    uint64_t get_pieces_by_char( const char character) const {
        switch (character) {
            case 'P': return get_white_pawns();
            case 'p': return get_black_pawns();
            case 'N': return get_white_knights();
            case 'n': return get_black_knights();
            case 'B': return get_white_bishops();
            case 'b': return get_black_bishops();
            case 'R': return get_white_rooks();
            case 'r': return get_black_rooks();
            case 'Q': return get_white_queens();
            case 'q': return get_black_queens();
            case 'K': return get_white_king();
            case 'k': return get_black_king();
            default: return '.';

        }
    };
    uint64_t& get_piece_bitboard_ref(char piece) {
        switch(piece) {
            case 'P': return white_pawn;
            case 'p': return black_pawn;
            case 'N': return white_knight;
            case 'n': return black_knight;
            case 'B': return white_bishop;
            case 'b': return black_bishop;
            case 'R': return white_rook;
            case 'r': return black_rook;
            case 'Q': return white_queen;
            case 'q': return black_queen;
            case 'K': return white_king;
            case 'k': return black_king;
            default:
                throw std::runtime_error(
                        std::string("Invalid piece for reference: '") + piece + "'"
                );
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