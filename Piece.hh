//
// Created by Kirill on 8/2/2025.
//

#ifndef PIECE_HH
#define PIECE_HH



class Piece {
    private:
    int square;
    char piece;
    int color;
    int defended;
    int attacked;
    int moved;


    public:
    void is_attacked();
    void is_defended();
    void has_moved();

};



#endif //PIECE_HH
