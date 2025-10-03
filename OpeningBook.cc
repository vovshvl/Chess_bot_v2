#include "OpeningBook.hh"
#include <iostream>
#include <vector>
#include <map>

// Define the static member
std::map<std::string, std::vector<std::string>> OpeningBook::book = {
        // First moves
        {"", {"e2e4", "d2d4", "c2c4", "g1f3"}},

        // 1.e4
        {"e2e4", {"e7e5", "c7c5", "e7e6", "c7c6"}},
        // 2nd ply responses
        {"e2e4e7e5", {"g1f3", "f1c4"}},               // King's Pawn openings: Italian, Ruy Lopez etc.
        {"e2e4c7c5", {"g1f3", "b1c3"}},              // Sicilian
        {"e2e4e7e6", {"d2d4"}},                      // French
        {"e2e4c7c6", {"d2d4"}},                      // Caro-Kann

        // 3rd ply
        {"e2e4e7e5g1f3", {"b8c6", "g8f6"}},          // Knight development
        {"e2e4e7e5f1c4", {"b8c6", "g8f6"}},

        // 4th ply
        {"e2e4e7e5g1f3b8c6", {"f1c4", "f1b5"}},      // Italian, Ruy Lopez setups
        {"e2e4e7e5f1c4b8c6", {"g1f3"}},

        // 5th ply
        {"e2e4e7e5g1f3b8c6f1c4", {"g8f6"}},          // Italian Game mainline
        {"e2e4e7e5g1f3b8c6f1b5", {"a7a6"}},          // Ruy Lopez mainline
        {"e2e4c7c5g1f3b8c6", {"d2d4"}},              // Sicilian Open
        {"e2e4c7c5b1c3g8f6", {"d2d4"}},

        // 1.d4
        {"d2d4", {"d7d5", "g8f6", "e7e6", "c7c5"}},
        {"d2d4d7d5", {"c2c4", "g1f3"}},              // Queen's Gambit
        {"d2d4g8f6", {"c2c4", "g1f3"}},              // Indian Defense

        // 2nd ply
        {"d2d4d7d5c2c4", {"e7e6", "c7c6"}},          // Queen's Gambit Declined / Accepted
        {"d2d4g8f6c2c4", {"e7e6", "g7g6"}},          // Indian defenses
        {"d2d4d7d5g1f3", {"g8f6"}},                  // Classical development
        {"d2d4g8f6g1f3", {"e7e6", "g7g6"}},

        // 3rd ply
        {"d2d4d7d5c2c4e7e6", {"g1f3", "b1c3"}},      // QGD mainline
        {"d2d4d7d5c2c4c7c6", {"g1f3", "b1c3"}},      // QGA mainline
        {"d2d4g8f6c2c4e7e6", {"g1f3", "b1c3"}},      // Nimzo-Indian setups
        {"d2d4g8f6c2c4g7g6", {"g1f3", "b1c3"}},      // King's Indian setups
};