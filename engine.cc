#include "engine.hh"

static UltraFastEvaluator g_ultra_evaluator;

inline int evaluate_position(const board& b, bool white_to_move = true) noexcept {
    return g_ultra_evaluator.evaluate(b, white_to_move);
}