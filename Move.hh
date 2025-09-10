#pragma once
#include <optional>

class Move_class {
private:
    int from_square;
    int to_square;
    std::optional<char> piece_captured;
    std::optional<char> promotion;
    bool castling_king;
    bool castling_queen;
    std::optional<int> en_passant_captured_square;
    std::optional<bool> check;
    int move_score;

public:
    // Konstruktor
    Move_class(int from_square,
         int to_square,
         std::optional<char> piece_captured = std::nullopt,
         std::optional<char> promotion = std::nullopt,
         bool castling_king = false,
         bool castling_queen = false,
         std::optional<int> en_passant_captured_square = std::nullopt,
         std::optional<bool> check = std::nullopt);

    // Getter
    int get_from_square() const;
    int get_to_square() const;
    int get_move_score() const;
    int get_captured_piece() const;

    // Methoden
    void give_move_score();
};
