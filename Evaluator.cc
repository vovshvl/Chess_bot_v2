#include "Evaluator.hh"

Evaluator g_ultra_evaluator;

int evaluate_position(const board& b, bool white_to_move) noexcept {
    return g_ultra_evaluator.evaluate(b, white_to_move);
}