#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace ehdu{

typedef uint64_t BitBoard;

enum CastlingRights{
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,
    KING_SIDE = WHITE_OO | BLACK_OO,
    QUEEN_SIDE = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,
    ANY_CASTLING = WHITE_CASTLING | BLACK_CASTLING,
    CASTLING_RIGHT_NB = 16
};

enum Color{
    WHITE, BLACK, COLOR_NB = 2
};

enum Direction: int{
    NORTH = 8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,
    NORTH_EAST = NORTH + EAST,
    NORTH_WEST = NORTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST
};

enum File: int{
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

enum GenType{
    CAPTURES,
    QUIETS,
    QUIET_CHECKS,
    EVASIONS,
    NON_EVASIONS,
    LEGAL
};

enum Move: int{
    MOVE_NONE,
    MOVE_NULL = 65
};

enum MoveType{
    NORMAL,
    PROMOTION = 1 << 14,
    EN_PASSANT = 2 << 14,
    CASTLING = 3 << 14
};

enum PieceType{
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0, PIECE_TYPE_NB = 8
};

enum Piece{
    NO_PIECE,
    W_PAWN = PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
};

enum Rank: int{
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,
    RANK_NB
};

enum Square: int{
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE, SQUARE_ZERO = 0, SQUARE_NB = 64
};

#define ENABLE_BASE_OPERATORS_ON(T) \
    constexpr T operator+(T d1, int d2){return T(int(d1) + d2);}

#define ENABLE_INCR_OPERATORS_ON(T) \
    inline T &operator++(T &d){return d = T(int(d) + 1);}  \
    inline T &operator--(T &d){return d = T(int(d) - 1);}

#define ENABLE_FULL_OPERATORS_ON(T) \
    ENABLE_BASE_OPERATORS_ON(T) \
    constexpr T operator*(int i, T d){return T(i * int(d));}

ENABLE_INCR_OPERATORS_ON(File)
ENABLE_INCR_OPERATORS_ON(Rank)
ENABLE_INCR_OPERATORS_ON(Square)
ENABLE_FULL_OPERATORS_ON(Direction)

#undef ENABLE_INCR_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON
#undef ENABLE_FULL_OPERATORS_ON

constexpr CastlingRights operator&(Color c, CastlingRights cr){
    return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr);
}

constexpr Color operator~(Color c){
    return Color(c ^ BLACK);
}

constexpr Square operator+(Square s, Direction d){
    return Square(int(s) + int(d));
}

constexpr Square operator-(Square s, Direction d){
    return Square(int(s) - int(d));
}

inline Square &operator+=(Square &s, Direction d){
    return s = s + d;
}

inline Square &operator-=(Square &s, Direction d){
    return s = s - d;
}

constexpr Color color_of(Piece pc){
    return Color(pc >> 3);
}

constexpr File file_of(Square s){
    return File(s & 7);
}

constexpr Square from_sq(Move m){
    return Square((m >> 6) & 0x3F);
}

constexpr bool is_ok(Square s){
    return s >= SQ_A1 && s <= SQ_H8;
}

constexpr Square to_sq(Move m){
    return Square(m & 0x3F);
}

constexpr PieceType type_of(Piece pc){
    return PieceType(pc & 7);
}

constexpr MoveType type_of(Move m){
    return MoveType(m & (3 << 14));
}

template<MoveType T>
constexpr Move make(Square from, Square to, PieceType pt = KNIGHT){
    return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
}

constexpr Move make_move(Square from, Square to){
    return Move((from << 6) + to);
}

constexpr Piece make_piece(Color c, PieceType pt){
    return Piece((c << 3) + pt);
}

constexpr Square make_square(File f, Rank r){
    return Square((r << 3) + f);
}

constexpr Direction pawn_push(Color c){
    return c == WHITE ? NORTH : SOUTH;
}

constexpr PieceType promotion_type(Move m){
    return PieceType(((m >> 12) & 3) + KNIGHT);
}

constexpr Rank rank_of(Square s){
    return Rank(s >> 3);
}

constexpr Rank relative_rank(Color c, Rank r){
    return Rank(r ^ (c * 7));
}

constexpr Rank relative_rank(Color c, Square s){
    return relative_rank(c, rank_of(s));
}

constexpr Square relative_square(Color c, Square s){
    return Square(s ^ (c * 56));
}

constexpr int MAX_MOVES = 256;

}

#endif // TYPES_H
