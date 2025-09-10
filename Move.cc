//
// Created by Kirill on 8/2/2025.
//
#include <utility>
#include <optional>
#include "Move.hh"


    Move_class::Move_class(int from_square,
         int to_square,
         std::optional<char> piece_captured,
         std::optional<char> promotion,
         bool castling_king,
         bool castling_queen,
         std::optional<int> en_passant_captured_square,
         std::optional<bool> check)
        : from_square(from_square),
          to_square(to_square),
          piece_captured(piece_captured),
          promotion(promotion),
          castling_king(castling_king),
          castling_queen(castling_queen),
          en_passant_captured_square(en_passant_captured_square),
          check(check),
          move_score(0) {}

    int Move_class::get_from_square() const { return from_square; }
    int Move_class::get_to_square() const { return to_square; }

    void Move_class::give_move_score() {
        if (check.has_value() && check.value()) {
            move_score += 1000;
        }
        if (piece_captured.has_value()) {
            move_score += 100;
        }
    }

    int Move_class::get_move_score() const { return move_score; }