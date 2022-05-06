#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.h"
#include <cmath>

namespace ehdu{

constexpr BitBoard Rank1BB = 0xFF;
constexpr BitBoard Rank2BB = Rank1BB << (8 * 1);
constexpr BitBoard Rank3BB = Rank1BB << (8 * 2);
constexpr BitBoard Rank4BB = Rank1BB << (8 * 3);
constexpr BitBoard Rank5BB = Rank1BB << (8 * 4);
constexpr BitBoard Rank6BB = Rank1BB << (8 * 5);
constexpr BitBoard Rank7BB = Rank1BB << (8 * 6);
constexpr BitBoard Rank8BB = Rank1BB << (8 * 7);

constexpr BitBoard FileABB = 0x0101010101010101ULL;
constexpr BitBoard FileBBB = FileABB << 1;
constexpr BitBoard FileCBB = FileABB << 2;
constexpr BitBoard FileDBB = FileABB << 3;
constexpr BitBoard FileEBB = FileABB << 4;
constexpr BitBoard FileFBB = FileABB << 5;
constexpr BitBoard FileGBB = FileABB << 6;
constexpr BitBoard FileHBB = FileABB << 7;

extern BitBoard PawnAttacks[COLOR_NB][SQUARE_NB];
extern BitBoard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];

extern uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];

extern BitBoard BetweenBB[SQUARE_NB][SQUARE_NB];
extern BitBoard LineBB[SQUARE_NB][SQUARE_NB];
extern BitBoard SquareBB[SQUARE_NB];

BitBoard operator&(BitBoard b, Square s);
BitBoard operator|(BitBoard b, Square s);
BitBoard operator^(BitBoard b, Square s);
BitBoard operator|=(BitBoard &b, Square s);
BitBoard operator^=(BitBoard &b, Square s);
BitBoard operator&(Square s, BitBoard b);
BitBoard operator|(Square s, BitBoard b);
BitBoard operator^(Square s, BitBoard b);
BitBoard operator|(Square s1, Square s2);

template<Direction D>
constexpr BitBoard shift(BitBoard b){
    if(D == NORTH){
        return b << 8;
    }
    if(D == SOUTH){
        return b >> 8;
    }
    if(D == NORTH + NORTH){
        return b << 16;
    }
    if(D == SOUTH + SOUTH){
        return b >> 16;
    }
    if(D == EAST){
        return (b & ~FileHBB) << 1;
    }
    if(D == WEST){
        return (b & ~FileABB) >> 1;
    }
    if(D == NORTH_EAST){
        return (b & ~FileHBB) << 9;
    }
    if(D == NORTH_WEST){
        return (b & ~FileABB) << 7;
    }
    if(D == SOUTH_EAST){
        return (b & ~FileHBB) >> 7;
    }
    if(D == SOUTH_WEST){
        return (b & ~FileABB) >> 9;
    }
    return 0;
}

constexpr BitBoard file_bb(File f){
    return FileABB << f;
}

constexpr BitBoard file_bb(Square s){
    return file_bb(file_of(s));
}

constexpr BitBoard adjacent_files_bb(Square s){
    return shift<EAST>(file_bb(s)) | shift<WEST>(file_bb(s));
}

template<PieceType Pt>
BitBoard attacks_bb(Square s){
    return PseudoAttacks[Pt][s];
}

BitBoard sliding_attack(PieceType pt, Square sq, BitBoard occupied);

template<PieceType Pt>
BitBoard attacks_bb(Square s, BitBoard occupied){
    switch(Pt){
    case BISHOP:
        return sliding_attack(BISHOP, s, occupied);
    case ROOK:
        return sliding_attack(ROOK, s, occupied);
    case QUEEN:
        return sliding_attack(BISHOP, s, occupied) | sliding_attack(ROOK, s, occupied);
    default:
        return PseudoAttacks[Pt][s];
    }
}

template<class T1 = Square>
inline int distance(Square x, Square y);

template<>
inline int distance<File>(Square x, Square y){
    return std::abs(file_of(x) - file_of(y));
}

template<>
inline int distance<Rank>(Square x, Square y){
    return std::abs(rank_of(x) - rank_of(y));
}

template<>
inline int distance<Square>(Square x, Square y){
    return SquareDistance[x][y];
}

constexpr BitBoard forward_ranks_bb(Color c, Square s){
    return c == WHITE ? ~Rank1BB << 8 * relative_rank(WHITE, s)
                      : ~Rank8BB << 8 * relative_rank(BLACK, s);
}

constexpr bool more_than_one(BitBoard b){
    return b & (b - 1);
}

template<Color C>
constexpr BitBoard pawn_attacks_bb(BitBoard b){
    return C == WHITE ? shift<NORTH_WEST>(b) | shift<NORTH_EAST>(b)
                      : shift<SOUTH_WEST>(b) | shift<SOUTH_EAST>(b);
}

constexpr BitBoard pawn_attack_span(Color c, Square s){
    return forward_ranks_bb(c, s) & adjacent_files_bb(s);
}

constexpr BitBoard rank_bb(Rank r){
    return Rank1BB << (8 * r);
}

constexpr BitBoard rank_bb(Square s){
    return rank_bb(rank_of(s));
}

bool aligned(Square s1, Square s2, Square s3);
BitBoard attacks_bb(PieceType pt, Square s, BitBoard occupied);
void bb_init();
BitBoard between_bb(Square s1, Square s2);
BitBoard line_bb(Square s1, Square s2);
Square lsb(BitBoard b);
BitBoard pawn_attacks_bb(Color c, Square s);
Square pop_lsb(BitBoard &b);
int popcount(BitBoard b);
BitBoard safe_destination(Square s, int step);
BitBoard square_bb(Square s);

}

#endif // BITBOARD_H
