# Chess Bot
### Important notes
1. Board is enumerated from a1(0) to h8(63) with b1(1) being the second element.
2. Castling works like normal kings legal move and moving the rook is handled in execute_move

### List to Do
1) Write tests ⚠️
2) Legal moves for each piece ✅
3) Castling ✅
4) En passant ❌
5) Pawn promotion ✅
6) Rework Move and piece ❌
7) Total legal moves ⚠️
   - checkmask legal moves
9) Checkmate ✅
10) ALPHA-BETA pruning ⚠️
    - works, but need move sorting(check, captures, development)
12) Evaluate Board ✅(Further optimization is possible but not needed)
13) reverse move fix(in cases of castling, promotion etc.)
14) King move fix(he takes with check)
