#ifndef STOCKFISH_H
#define STOCKFISH_H

#include "BitBoard.h"

#include <string>

namespace ehdu{

class Position{
public:
    static std::string move(Move m);
    static std::string square(Square s);
    Position();
    virtual ~Position() = default;
    BitBoard blockers_for_king(Color c) const;
    bool can_castle(CastlingRights cr) const;
    bool castling_impeded(CastlingRights cr) const;
    Square castling_rook_square(CastlingRights cr) const;
    BitBoard check_squares(PieceType pt) const;
    BitBoard checkers() const;
    Square ep_square() const;
    std::string fen() const;
    bool legal(Move m) const;
    BitBoard pieces(PieceType pt = ALL_PIECES) const;
    BitBoard pieces(Color c) const;
    BitBoard pieces(Color c, PieceType pt) const;
    Color side_to_move() const;
    template<PieceType Pt>
    Square square(Color c) const{
        return lsb(pieces(c, Pt));
    }
public:  // FIXME
    void do_move(Move m);
    Piece piece_on(Square s) const;
    void set(const std::string &fenStr);
    Move to_move(std::string &str);
private:
    BitBoard attackers_to(Square s) const;
    BitBoard attackers_to(Square s, BitBoard occupied) const;
    template<bool Do>
    void do_castling(Color us, Square from, Square &to, Square &rfrom, Square &rto){
        bool kingSide = to > from;
        rfrom = to;
        rto = relative_square(us, kingSide ? SQ_F1 : SQ_D1);
        to = relative_square(us, kingSide ? SQ_G1 : SQ_C1);
        remove_piece(Do ? from : to);
        remove_piece(Do ? rfrom : rto);
        board[Do ? from : to] = board[Do ? rfrom : rto] = NO_PIECE;
        put_piece(make_piece(us, KING), Do ? to : from);
        put_piece(make_piece(us, ROOK), Do ? rto : rfrom);
    }
    void do_move(Move m, bool givesCheck);
    bool empty(Square s) const;
    bool gives_check(Move m) const;
    void move_piece(Square from, Square to);
    Piece moved_piece(Move m) const;
    void put_piece(Piece pc, Square s);
    void remove_piece(Square s);
    void set_castling_right(Color c, Square rfrom);
    void set_check_info();
    BitBoard slider_blockers(BitBoard sliders, Square s, BitBoard &pinners) const;
    BitBoard pieces(PieceType pt1, PieceType pt2) const;
    BitBoard pieces(Color c, PieceType pt1, PieceType pt2) const;
    Piece board[SQUARE_NB];
    Color sideToMove;
    BitBoard byColorBB[COLOR_NB];
    BitBoard byTypeBB[PIECE_TYPE_NB];
    int castlingRightsMask[SQUARE_NB];
    Square castlingRookSquare[CASTLING_RIGHT_NB];
    BitBoard castlingPath[CASTLING_RIGHT_NB];
    int castlingRights;
    Square epSquare;
    BitBoard checkersBB;
    BitBoard blockersForKing[COLOR_NB];
    BitBoard pinners[COLOR_NB];
    BitBoard checkSquares[PIECE_TYPE_NB];
};

}

#endif // STOCKFISH_H
