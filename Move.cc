//
// Created by Kirill on 8/2/2025.
//
#include <utility>
#include <optional>

class Move {
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
    Move(int from_square,
         int to_square,
         std::optional<char> piece_captured = std::nullopt,
         std::optional<char> promotion = std::nullopt,
         bool castling_king = false,
         bool castling_queen = false,
         std::optional<int> en_passant_captured_square = std::nullopt,
         std::optional<bool> check = std::nullopt)
        : from_square(from_square),
          to_square(to_square),
          piece_captured(piece_captured),
          promotion(promotion),
          castling_king(castling_king),
          castling_queen(castling_queen),
          en_passant_captured_square(en_passant_captured_square),
          check(check),
          move_score(0) {}

    int get_from_square() const { return from_square; }
    int get_to_square() const { return to_square; }

    void give_move_score() {
        if (check.has_value() && check.value()) {
            move_score += 1000;
        }
        if (piece_captured.has_value()) {
            move_score += 100;
        }
    }
    int get_move_score() const { return move_score; }
};