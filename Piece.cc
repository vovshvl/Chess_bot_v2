#include <vector>
#include <algorithm>
#include "board.hh"
#include "Evaluator.hh"
#include <cstdint>
#include <bit>
//fixed implementation to avoid undefined behavior
class BitUtils {
public:
    static inline int popcount(uint64_t bb) noexcept {
#if __cpp_lib_bitops >= 201907L
        return std::popcount(bb);
#else
        return __builtin_popcountll(bb);
#endif
    }


    static inline int lsb(uint64_t bb) noexcept {
        if (bb == 0) return -1;
#if __cpp_lib_bitops >= 201907L
        return std::countr_zero(bb);
#else
        return __builtin_ctzll(bb);
#endif
    }


    static inline int msb(uint64_t bb) noexcept {
        if (bb == 0) return -1;
#if __cpp_lib_bitops >= 201907L
        return 63 - std::countl_zero(bb);
#else
        return 63 - __builtin_clzll(bb);
#endif
    }
};

class tables
{
public:
    using Bitboard = uint64_t;
    //ToDO generate masks (probably in scrap functions)
    static std::array<uint64_t,64> rook_moves;
    static std::array<Bitboard,64> bishop_moves;
    Bitboard get_bishop_moves(int sq) noexcept
    {
        return 0;
    }
};

class Piece {
private:
    // Bitboard type for representing piece positions and attack patterns
    using Bitboard = uint64_t;

    // Bitboard constants for empty and full boards
    static constexpr Bitboard EMPTY = 0ULL; // All bits set to 0
    static constexpr Bitboard FULL = ~0ULL; // All bits set to 1

    // Basic piece properties
    bool has_moved = false; // Tracks if piece has moved (important for pawns and castling)
    int square; // Current square (0-63, a1=0, h8=63)
    int value; // Piece value (+/- for white/black)
    bool is_white = value > 0;

    // Bitboards for current piece's attack and move patterns
    Bitboard attacks; // Squares this piece attacks
    Bitboard moves; // Legal moves for this piece

    // File masks for move generation
    static constexpr Bitboard FILE_A = 0x0101010101010101ULL; // Leftmost file
    static constexpr Bitboard FILE_B = FILE_A << 1;
    static constexpr Bitboard FILE_G = FILE_A << 6;
    static constexpr Bitboard FILE_H = FILE_A << 7;// Rightmost file


public:
    static bool is_attacked(const board&  chess_board, int square,bool is_white) {

        // Get all opponent pieces
        Bitboard opponent_pawns = chess_board.get_pieces_by_value(is_white ? -1 : 1);
        Bitboard opponent_knights = chess_board.get_pieces_by_value(is_white ? -2 : 2);
        Bitboard opponent_bishops = chess_board.get_pieces_by_value(is_white ? -3 : 3);
        Bitboard opponent_rooks = chess_board.get_pieces_by_value(is_white ? -4 : 4);
        Bitboard opponent_queens = chess_board.get_pieces_by_value(is_white ? -5 : 5);
        Bitboard opponent_king = chess_board.get_pieces_by_value(is_white ? -6 : 6);

        if (pawn_attacks(square, !is_white) & opponent_pawns)
            return true;

        if (knight_attacks(square) & opponent_knights)
            return true;

        Bitboard bishop_queens = opponent_bishops | opponent_queens;
        if (bishop_attacks(square,is_white, chess_board) & bishop_queens)
            return true;

        Bitboard rook_queens = opponent_rooks | opponent_queens;
        if (rook_attacks(square, chess_board) & rook_queens)
            return true;
        if (king_attacks(square) & opponent_king)
            return true;

        return false;
    }

    static std::vector<std::pair<int, char>> attackers(int sq, bool is_white, const board& chess_board){
        Bitboard all_pieces = chess_board.get_all_pieces();
        Bitboard enemy_pieces = 0;
        std::vector<std::pair<int, char>> result;

        Bitboard checkmask = queen_attacks(sq, is_white, chess_board) | knight_attacks(sq);

        if(is_white){
            enemy_pieces = chess_board.get_black_pieces();
        }
        else{
            enemy_pieces = chess_board.get_white_pieces();
        }
        if((checkmask | enemy_pieces) == 0ULL){
            //not under attack
            //Consider attacks, because if the pawn is standing on the diagonal there will be no optimisation, although it is not attacking
            return {};
        }
        Bitboard enemy_pawns   = is_white ? chess_board.get_black_pawns()   : chess_board.get_white_pawns();
        Bitboard enemy_knights = is_white ? chess_board.get_black_knights() : chess_board.get_white_knights();
        Bitboard enemy_bishops = is_white ? chess_board.get_black_bishops() : chess_board.get_white_bishops();
        Bitboard enemy_rooks   = is_white ? chess_board.get_black_rooks()   : chess_board.get_white_rooks();
        Bitboard enemy_queens  = is_white ? chess_board.get_black_queens()  : chess_board.get_white_queens();
        Bitboard enemy_king    = is_white ? chess_board.get_black_king()    : chess_board.get_white_king();

        Bitboard pawn_attackers = enemy_pawns & pawn_attacks(sq, is_white);
        Bitboard knight_attackers = enemy_knights & knight_attacks(sq);
        Bitboard bishop_attackers = enemy_bishops & bishop_attacks(sq,is_white, chess_board);
        Bitboard rook_attackers = enemy_rooks  & rook_attacks(sq, chess_board); // !is_white sus
        Bitboard queen_attackers = enemy_queens & queen_attacks(sq,is_white, chess_board);
        Bitboard king_attackers   = king_attacks(sq) & enemy_king;

        auto add_attackers = [&](Bitboard bb, char piece_char) {
            // Adjust for color: uppercase if white, lowercase if black
            char final_char = is_white ? tolower(piece_char) : toupper(piece_char);
            while (bb) {
                int from_sq = BitUtils::lsb(bb); // index of LS1B
                result.push_back({from_sq, final_char});
                bb &= bb - 1; // clear LS1B
            }
        };

        add_attackers(pawn_attackers, 'P');
        add_attackers(knight_attackers, 'N');
        add_attackers(bishop_attackers, 'B');
        add_attackers(rook_attackers, 'R');
        add_attackers(king_attackers, 'K');
        add_attackers(queen_attackers, 'Q');

        return result;

    }
    static std::vector<Move> legal_moves(board chess_board, bool is_white) {
        std::vector<Move> all_moves;
        Bitboard knight;
        Bitboard bishop;
        Bitboard rook;
        Bitboard queen;
        Bitboard king;
        Bitboard pawns;
        if (is_white) {
            knight = chess_board.get_white_knights();
            bishop = chess_board.get_white_bishops();
            rook = chess_board.get_white_rooks();
            queen = chess_board.get_white_queens();
            king = chess_board.get_white_king();
            pawns = chess_board.get_white_pawns();

        }
        else {
            knight = chess_board.get_black_knights();
            bishop = chess_board.get_black_bishops();
            rook = chess_board.get_black_rooks();
            queen = chess_board.get_black_queens();
            king = chess_board.get_black_king();
            pawns = chess_board.get_black_pawns();
        }
        if (is_in_check(chess_board, is_white)) {
            Bitboard king_board = 0;
            if(is_white){
                king_board = chess_board.get_white_king();
            }
            else{
                king_board = chess_board.get_black_king();
            }
            int king_sq = __builtin_ctzll(king_board);
            std::vector<std::pair<int, char>> checkers = attackers(king_sq, is_white, chess_board);
            int num_checkers = checkers.size();
            std::vector<int> possible_king_moves_sq = bitboard_to_array(king_moves(king_sq, is_white, chess_board));
            for (int pos_sq : possible_king_moves_sq) {
                Move move = {king_sq, pos_sq};
                char target_piece = chess_board.get_piece_at_square(pos_sq);
                if (target_piece == 'K' || target_piece == 'k') continue;
                all_moves.push_back(move);}

            if(num_checkers ==1){
                //besides running away we can protect the king with other pieces
                Bitboard checkmask = 0;
                auto [checker_sq, checker_char] = checkers[0];

                if (checker_char == 'r' || checker_char == 'R' || checker_char == 'b' || checker_char == 'B' || checker_char == 'q' || checker_char == 'Q'){
                    checkmask = chess_board.squares_between(checker_sq, king_sq) | (1ULL << checker_sq); //To include capture
                }
                else{
                    checkmask = (1ULL << checker_sq); //For knights and pawns only capture works
                }
                //ways to protect
                for (int piece_sq : bitboard_to_array(knight)) {
                    Bitboard moves_bb = knight_moves(piece_sq, is_white, chess_board);
                    moves_bb &= checkmask;  // only moves that capture/block the check
                    for (int pos_sq : bitboard_to_array(moves_bb)) {
                        all_moves.push_back({piece_sq, pos_sq});
                    }
                }
                for (int piece_sq : bitboard_to_array(pawns)) {
                    Bitboard moves_bb = pawn_moves(piece_sq, is_white, chess_board);
                    moves_bb &= checkmask;
                    for (int pos_sq : bitboard_to_array(moves_bb)) {
                        char target_piece = chess_board.get_piece_at_square(pos_sq);
                        if (target_piece == 'K' || target_piece == 'k') continue;
                        if((is_white && pos_sq >=56) || (!is_white && pos_sq<=7)){ //Promotion
                            all_moves.push_back({piece_sq, pos_sq, 'q'});
                            all_moves.push_back({piece_sq, pos_sq, 'r'});
                            all_moves.push_back({piece_sq, pos_sq, 'n'});
                            all_moves.push_back({piece_sq, pos_sq, 'b'});
                        }
                        else{
                            all_moves.push_back({piece_sq, pos_sq}); //normal move
                        }
                    }
                }
                for (int piece_sq : bitboard_to_array(bishop)) {
                    Bitboard moves_bb = bishop_moves(piece_sq, is_white, chess_board);
                    moves_bb &= checkmask;
                    for (int pos_sq : bitboard_to_array(moves_bb)) {
                        char target_piece = chess_board.get_piece_at_square(pos_sq);
                        if (target_piece == 'K' || target_piece == 'k') continue;
                        all_moves.push_back({piece_sq, pos_sq});
                    }
                }
                for (int piece_sq : bitboard_to_array(rook)) {
                    Bitboard moves_bb = rook_moves(piece_sq, is_white, chess_board);
                    moves_bb &= checkmask;
                    for (int pos_sq : bitboard_to_array(moves_bb)) {
                        char target_piece = chess_board.get_piece_at_square(pos_sq);
                        if (target_piece == 'K' || target_piece == 'k') continue;
                        all_moves.push_back({piece_sq, pos_sq});
                    }
                }
                for (int piece_sq : bitboard_to_array(queen)) {
                    Bitboard moves_bb = queen_moves(piece_sq, is_white, chess_board);
                    moves_bb &= checkmask;
                    for (int pos_sq : bitboard_to_array(moves_bb)) {
                        char target_piece = chess_board.get_piece_at_square(pos_sq);
                        if (target_piece == 'K' || target_piece == 'k') continue;
                        all_moves.push_back({piece_sq, pos_sq});
                    }
                }

            }



        }
        else {
            std::vector<int> knights_sq = bitboard_to_array(knight); //potentially uneffizient
            for (int knight_sq : knights_sq) {
                std::vector<int> possible_knight_moves_sq = bitboard_to_array(knight_moves(knight_sq, is_white, chess_board));
                for (int pos_sq : possible_knight_moves_sq) {
                    char target_piece = chess_board.get_piece_at_square(pos_sq);
                    if (target_piece == 'K' || target_piece == 'k') continue;
                    Move move = {knight_sq, pos_sq};
                    all_moves.push_back(move);
                }
            }
            std::vector<int> pawns_sq = bitboard_to_array(pawns);
            for (int pawn_sq : pawns_sq) {
                std::vector<int> possible_pawn_moves_sq = bitboard_to_array(pawn_moves(pawn_sq, is_white, chess_board));
                for (int pos_sq : possible_pawn_moves_sq) {
                    char target_piece = chess_board.get_piece_at_square(pos_sq);
                    if (target_piece == 'K' || target_piece == 'k') continue;
                    if((is_white && pos_sq >=56) || (!is_white && pos_sq<=7)){ //Promotion
                        all_moves.push_back({pawn_sq, pos_sq, 'q'});
                        all_moves.push_back({pawn_sq, pos_sq, 'r'});
                        all_moves.push_back({pawn_sq, pos_sq, 'n'});
                        all_moves.push_back({pawn_sq, pos_sq, 'b'});
                    }
                    else{
                        all_moves.push_back({pawn_sq, pos_sq}); //normal move
                    }
                }
            }
            std::vector<int> bishops_sq = bitboard_to_array(bishop);
            for (int bishop_sq : bishops_sq) {
                std::vector<int> possible_bishop_moves_sq = bitboard_to_array(bishop_moves(bishop_sq, is_white, chess_board));
                for (int pos_sq : possible_bishop_moves_sq) {
                    char target_piece = chess_board.get_piece_at_square(pos_sq);
                    if (target_piece == 'K' || target_piece == 'k') continue;
                    Move move = {bishop_sq, pos_sq};
                    all_moves.push_back(move);
                }
            }
            std::vector<int> rooks_sq = bitboard_to_array(rook);
            for (int rook_sq : rooks_sq) {
                std::vector<int> possible_rook_moves_sq = bitboard_to_array(rook_moves(rook_sq, is_white, chess_board));
                for (int pos_sq : possible_rook_moves_sq) {
                    char target_piece = chess_board.get_piece_at_square(pos_sq);
                    if (target_piece == 'K' || target_piece == 'k') continue;
                    Move move = {rook_sq, pos_sq};
                    all_moves.push_back(move);
                }
            }
            std::vector<int> queens_sq = bitboard_to_array(queen);
            for (int queen_sq : queens_sq) {
                std::vector<int> possible_queen_moves_sq = bitboard_to_array(queen_moves(queen_sq, is_white, chess_board));
                for (int pos_sq : possible_queen_moves_sq) {
                    char target_piece = chess_board.get_piece_at_square(pos_sq);
                    if (target_piece == 'K' || target_piece == 'k') continue;
                    Move move = {queen_sq, pos_sq};
                    all_moves.push_back(move);
                }
            }
            int king_sq = get_square_by_bitboard(king);
            std::vector<int> possible_king_moves_sq = bitboard_to_array(king_moves(king_sq, is_white, chess_board));
                for (int pos_sq : possible_king_moves_sq) {
                    char target_piece = chess_board.get_piece_at_square(pos_sq);
                    if (target_piece == 'K' || target_piece == 'k') continue;
                    Move move = {king_sq, pos_sq};
                    all_moves.push_back(move);
                }
        }

        return all_moves;
    }
    static bool is_in_check(const board& board, bool white) {
        int king_square =-1;
        if (white) {
            Bitboard wk = board.get_white_king();
            king_square = BitUtils::lsb(wk);
        } else {
            Bitboard bk = board.get_black_king();
            king_square = BitUtils::lsb(bk);
        }
        if (king_square == -1) return false;
        return is_attacked(board, king_square, white);
    }
    static bool is_mate(const board& board, bool is_white){
        if(is_in_check(board, is_white)){
            if(legal_moves(board, is_white).empty()){return true;}
        }
        return false;
    }

    static Bitboard get_bitboard_by_square(int square) {
        return 1ULL << square;
    }
    static std::vector<int> bitboard_to_array(Bitboard bb) {
        std::vector<int> squares;
        while (bb) {
            int sq = BitUtils::lsb(bb);
            if (sq == -1) break;
            squares.push_back(sq);
            bb &= bb - 1;
        }
        return squares;
    }
    static int get_square_by_bitboard(Bitboard bb) {
        return BitUtils::lsb(bb);
    }

    static Bitboard pawn_attacks(int sq, bool is_white) {
        Bitboard bb = 1ULL << sq;

        return is_white
                   ? ((bb << 7) & ~FILE_H) | ((bb << 9) & ~FILE_A)
                   : ((bb >> 7) & ~FILE_A) | ((bb >> 9) & ~FILE_H);
    }

    static Bitboard knight_attacks(int sq)  {
        Bitboard bb = 1ULL << sq;
        //To do check implementation, doesnt account for friendly figures
        return (((bb << 15) | (bb >> 17)) & ~FILE_H) |
               (((bb << 17) | (bb >> 15)) & ~FILE_A) |
               (((bb << 6) | (bb >> 10)) & ~(FILE_G | FILE_H)) |
               (((bb << 10) | (bb >> 6)) & ~(FILE_A | FILE_B));
    }

    static Bitboard new_bishop_attacks(int sq, bool is_white, const board& board){
        Bitboard bb = 1ULL << sq;
        return 0;
    }

    static Bitboard bishop_attacks(int sq, bool is_white, const board& chess_board) {
        Bitboard attacks = EMPTY;
        const int r = sq / 8, f = sq % 8;
        const Bitboard our_pieces = is_white ? chess_board.get_white_pieces() : chess_board.get_black_pieces();
        const Bitboard all_pieces = chess_board.get_all_pieces();

        for (int rank = r - 1, file = f - 1; rank >= 0 && file >= 0; --rank, --file) {
            const int target_sq = rank * 8 + file;
            const Bitboard target_bb = 1ULL << target_sq;
            attacks |= target_bb;
            if (all_pieces & target_bb) break;
        }


        for (int rank = r - 1, file = f + 1; rank >= 0 && file < 8; --rank, ++file) {
            const int target_sq = rank * 8 + file;
            const Bitboard target_bb = 1ULL << target_sq;
            attacks |= target_bb;
            if (all_pieces & target_bb) break;
        }


        for (int rank = r + 1, file = f - 1; rank < 8 && file >= 0; ++rank, --file) {
            const int target_sq = rank * 8 + file;
            const Bitboard target_bb = 1ULL << target_sq;
            attacks |= target_bb;
            if (all_pieces & target_bb) break;
        }


        for (int rank = r + 1, file = f + 1; rank < 8 && file < 8; ++rank, ++file) {
            const int target_sq = rank * 8 + file;
            const Bitboard target_bb = 1ULL << target_sq;
            attacks |= target_bb;
            if (all_pieces & target_bb) break;
        }

        return attacks & ~our_pieces;
    }

    static Bitboard rook_attacks(int sq,const board& chess_board) {
        //To do check implementation, doesnt account for friendly figures
        Bitboard attacks = EMPTY;
        Bitboard occupied = chess_board.get_all_pieces();
        int r = sq / 8, f = sq % 8;
        for (int d: {-1, 1}) {
            int rank = r + d;
            while (rank >= 0 && rank < 8) {
                int target_sq = rank * 8 + f;
                attacks |= 1ULL << target_sq;
                if (occupied & (1ULL << target_sq)) break;
                rank += d;
            }
            int file = f + d;
            while (file >= 0 && file < 8) {
                int target_sq = r * 8 + file;
                attacks |= 1ULL << target_sq;
                if (occupied & (1ULL << target_sq)) break;
                file += d;
            }
        }
        return attacks;
    }

    static Bitboard king_attacks(int sq) {
        Bitboard bb = 1ULL << sq;

        return ((bb << 1) & ~FILE_A) | ((bb >> 1) & ~FILE_H) |
               (bb << 8) | (bb >> 8) |
               ((bb << 7) & ~FILE_H) | ((bb << 9) & ~FILE_A) |
               ((bb >> 7) & ~FILE_A) | ((bb >> 9) & ~FILE_H);
    }

    static Bitboard queen_attacks(int sq,bool is_white, board chess_board) {//
        return bishop_attacks(sq,is_white, chess_board) | rook_moves(sq,is_white,chess_board );
    }

    static Bitboard pawn_moves(int sq, bool is_white,const board& board) { //need to redo
        Bitboard bb = 1ULL << sq;
        Bitboard occupied = is_white ? board.get_black_pieces() : board.get_white_pieces();
        Bitboard all_occupied = board.get_all_pieces();
        Bitboard moves = 0ULL;

        //single push
        Bitboard single_push = is_white ? (bb << 8) : (bb >> 8);
        moves |= single_push & ~all_occupied;

        //double push
        Bitboard double_push;
        if (is_white && (8 <= sq && sq <= 15)) {
            double_push = bb << 16;
            if ((single_push & ~all_occupied) && (double_push & ~all_occupied)) {
                moves |= double_push;
            }
        }
        else if (!is_white && (48 <= sq && sq <= 55)) {
            double_push = bb >> 16;
            if ((single_push & ~all_occupied) && (double_push & ~all_occupied)) {
                moves |= double_push;
            }
        }

        return moves  | pawn_attacks(sq, is_white) & occupied;
    }

    static Bitboard knight_moves(int sq,bool is_white,const board& board) {
        //To do check implementation, doesnt account for friendly figures
        Bitboard occupied = is_white ? board.get_white_pieces() : board.get_black_pieces();
        return knight_attacks(sq) & ~occupied;
    }

    static Bitboard bishop_moves(int sq, bool is_white, const board& board) {
        //To do check implementation, doesnt account for friendly figures
        return bishop_attacks(sq,is_white, board);
    }

    static Bitboard rook_moves(int sq, bool is_white, const board& board) { //Explanation on  https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks#Sliding_Attacks_by_Calculation
        //To do check implementation, doesnt account for friendly figures
        int rank = sq/8;
        int file = sq % 8;

        uint8_t rank_occupied = board.get_rank(board.get_all_pieces(), rank); //=o
        uint8_t rank_slider = 1 << file; //=s
        uint8_t line_attacks_rank = (rank_occupied - 2*rank_slider) ^ board.reverse8(board.reverse8(rank_occupied) - 2*board.reverse8(rank_slider));

        uint8_t file_occupied = board.get_file(board.get_all_pieces(), file);//=o
        uint8_t file_slider = 1 << rank;//=s
        uint8_t line_attacks_file = (file_occupied - 2*file_slider) ^ board.reverse8(board.reverse8(file_occupied) - 2*(1 << (7 - rank)));


        uint64_t  moves = board.rank_to_bitboard(line_attacks_rank, rank) | board.file_to_bitboard(line_attacks_file, file);
        uint64_t own_pieces = is_white ? board.get_white_pieces() : board.get_black_pieces();
        return moves & ~own_pieces;

    }

    static Bitboard queen_moves(int sq, bool is_white,const board& board) {
        //To do check implementation, doesnt account for friendly figures
        return queen_attacks(sq, is_white,board);
    }

    static Bitboard castling(bool is_white, const board& board){
        Bitboard result = 0;
        Bitboard all_pieces = board.get_all_pieces();

        if(is_white){
            // King-side
            if(board.get_king_castle_white() && (board.white_rook & (1ULL << 7))){
                // Squares between king and rook must be empty
                if((all_pieces & ((1ULL << 5) | (1ULL << 6))) == 0ULL &&
                   !is_attacked(board,5,true) && !is_attacked(board,6,true) &&
                   !is_in_check(board,true)) {
                    result |= 1ULL << 6; // g1
                }
            }
            // Queen-side
            if(board.get_queen_castle_white() && (board.white_rook & (1ULL << 0))){
                if((all_pieces & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) == 0ULL &&
                   !is_attacked(board,2,true) && !is_attacked(board,3,true) &&
                   !is_in_check(board,true)) {
                    result |= 1ULL << 2; // c1
                }
            }
        } else { // black
            if(board.get_king_castle_black() && (board.black_rook & (1ULL << 63))){
                if((all_pieces & ((1ULL << 61) | (1ULL << 62))) == 0ULL &&
                   !is_attacked(board,61,false) && !is_attacked(board,62,false) &&
                   !is_in_check(board,false)) {
                    result |= 1ULL << 62; // g8
                }
            }
            if(board.get_queen_castle_black() && (board.black_rook & (1ULL << 56))){
                if((all_pieces & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) == 0ULL &&
                   !is_attacked(board,58,false) && !is_attacked(board,59,false) &&
                   !is_in_check(board,false)) {
                    result |= 1ULL << 58; // c8
                }
            }
        }

        return result;
    }



    static Bitboard king_moves(int sq, bool is_white, const board& board) {
        Bitboard moves = king_attacks(sq);
        Bitboard own_pieces = is_white ? board.get_white_pieces() : board.get_black_pieces();
        moves &= ~own_pieces;
        Bitboard safe_moves = EMPTY;
        for (int target_sq : bitboard_to_array(moves)) {
            if (!is_attacked(board, target_sq, is_white)) {
                safe_moves |= 1ULL << target_sq;
            }
        }

        return safe_moves | castling(is_white, board);
    }

/*
    static Bitboard castling(bool is_white, const board& board){
        Bitboard kingside_castling = 0;
        Bitboard queenside_castling = 0;
        Bitboard result = 0;

        //Getting mask where the bishops, knights and queen is standing, then AND with actually pieces
        if (is_white) {
            kingside_castling |= (1ULL << 5); // f1
            kingside_castling |= (1ULL << 6); // g1
            queenside_castling |= (1ULL << 1); // b1
            queenside_castling |= (1ULL << 2); // c1
            queenside_castling |= (1ULL << 3); // d1
        } else {
            kingside_castling |= (1ULL << 61); // f8
            kingside_castling |= (1ULL << 62); // g8
            queenside_castling |= (1ULL << 57); // b8
            queenside_castling |= (1ULL << 58); // c8
            queenside_castling |= (1ULL << 59); // d8
        }

        Bitboard all_pieces = board.get_all_pieces();

        bool king_castle_flag = is_white? board.get_king_castle_white() : board.get_king_castle_black();
        bool queen_castle_flag = is_white? board.get_queen_castle_white() : board.get_queen_castle_black();

        if((!king_castle_flag and !queen_castle_flag) or is_in_check(board, is_white)){return 0ULL;}

        if(((all_pieces & kingside_castling) == 0ULL) and king_castle_flag){
            if(!is_attacked(board, 5, is_white) and !is_attacked(board, 6, is_white)){
                result |= (1ULL << 6);
            }
        }
        if(((all_pieces & queenside_castling) == 0ULL) and queen_castle_flag){
            if(!is_attacked(board, 3, is_white) and !is_attacked(board, 2, is_white)){
                result |= (1ULL << 2);
            }
        }
        if(((all_pieces & kingside_castling) == 0ULL) and king_castle_flag){
            if(!is_attacked(board, 61, is_white) and !is_attacked(board, 62, is_white)){
                result |= (1ULL << 62);
            }
        }
        if(((all_pieces & queenside_castling) == 0ULL) and queen_castle_flag){
            if(!is_attacked(board, 59, is_white) and !is_attacked(board, 58, is_white)){
                result |= (1ULL << 58);
            }
        }
        return result;

    }

*/


    static Bitboard is_defended(int sq, bool is_white,const board& chess_board) {
        return is_attacked(chess_board, sq,!is_white);
    }

    static void sort_moves(std::vector<Move>& moves, board& board, bool is_white){
        auto piece_value = [](char piece) -> int {
            switch(toupper(piece)) {
                case 'P': return 100;
                case 'N': return 320;
                case 'B': return 330;
                case 'R': return 500;
                case 'Q': return 900;
                case 'K': return 10000;
                case '.': return 0;
            }
            return 0;
        };
        auto attcker_bitboard = [](char piece, int to, bool is_white, class board& board) -> Bitboard {
            switch(toupper(piece)) {
                case 'P': return pawn_attacks(to, is_white);
                case 'N': return knight_attacks(to);
                case 'B': return bishop_attacks(to, is_white, board);
                case 'R': return rook_attacks(to, board);
                case 'Q': return queen_attacks(to, is_white, board);
            }
            return 0;
        };


        std::vector<std::pair<Move, int>> scored_moves; //Prioritize check, captures, development?
        Bitboard enemy_king = is_white? board.get_black_king() : board.get_white_king();

        for(auto move : moves){
            int from = move.from;
            int to = move.to;
            char captured_piece = board.get_piece_at_square(to);
            char moving_piece = board.get_piece_at_square(from);
            int score = 0;

            //board.execute_move(from, to);
            //if(is_in_check(board, !is_white)) score+=100;
            //board.reverse_move(from, to, captured_piece);


            if(attcker_bitboard(moving_piece, to, is_white, board) & enemy_king) score +=500; //Check bonus


            if(captured_piece != '.'){ //Capture bonus
                score += piece_value(captured_piece)*10 - piece_value(moving_piece);
            }

            if(move.promotion != '\0'){ //Promotion bonus
                if(is_white){
                    switch(move.promotion){
                        case 'q': score+= 900; break;
                        case 'r': score+= 500; break;
                        case 'b': score+= 330; break;
                        case 'n': score+= 320; break;

                    }
                }
                if(!is_white){
                    switch(move.promotion){
                        case 'q': score-= 900; break;
                        case 'r': score-= 500; break;
                        case 'b': score-= 330; break;
                        case 'n': score-= 320; break;

                    }
                }
            }

            if (is_white) score += 10; else score -= 10; //tempo bonus

            //mobility bonus
            uint64_t knights = is_white ? board.get_white_knights() : board.get_black_knights();
            while (knights) {
                int sq = Evaluator::lsb(knights);
                score += Evaluator::popcount(knight_moves(sq, is_white, board)) * 5;
                knights = Evaluator::clear_lsb(knights);
            }
            uint64_t bishops = is_white ? board.get_white_bishops() : board.get_black_bishops();
            while (bishops) {
                int sq = Evaluator::lsb(bishops);
                score += Evaluator::popcount(bishop_moves(sq, is_white, board)) * 5;
                bishops = Evaluator::clear_lsb(bishops);
            }


            scored_moves.push_back({move, score});
        }
        if(is_white){
            // White wants largest first
            std::sort(scored_moves.begin(), scored_moves.end(),
                      [](auto a, auto b){ return a.second > b.second; });
        } else {
            // Black wants smallest first
            std::sort(scored_moves.begin(), scored_moves.end(),
                      [](auto a, auto b){ return a.second < b.second; });
        }
        moves.clear();
        for (auto& m : scored_moves) moves.push_back(m.first);
    }
};
