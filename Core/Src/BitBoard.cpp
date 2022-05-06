#include "BitBoard.h"

#include <bitset>
#include <initializer_list>

using namespace std;

namespace ehdu{

BitBoard PawnAttacks[COLOR_NB][SQUARE_NB];
BitBoard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];

uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];

BitBoard BetweenBB[SQUARE_NB][SQUARE_NB];
BitBoard LineBB[SQUARE_NB][SQUARE_NB];
BitBoard SquareBB[SQUARE_NB];

BitBoard operator&(BitBoard b, Square s){
    return b & square_bb(s);
}

BitBoard operator|(BitBoard b, Square s){
    return b | square_bb(s);
}

BitBoard operator^(BitBoard b, Square s){
    return b ^ square_bb(s);
}

BitBoard operator|=(BitBoard &b, Square s){
    return b |= square_bb(s);
}

BitBoard operator^=(BitBoard &b, Square s){
    return b ^= square_bb(s);
}

BitBoard operator&(Square s, BitBoard b){
    return b & s;
}

BitBoard operator|(Square s, BitBoard b){
    return b | s;
}

BitBoard operator^(Square s, BitBoard b){
    return b ^ s;
}

BitBoard operator|(Square s1, Square s2){
    return square_bb(s1) | s2;
}

bool aligned(Square s1, Square s2, Square s3){
    return line_bb(s1, s2) & s3;
}

BitBoard attacks_bb(PieceType pt, Square s, BitBoard occupied){
    switch(pt){
    case BISHOP:
        return attacks_bb<BISHOP>(s, occupied);
    case ROOK:
    	return attacks_bb<ROOK>(s, occupied);
    case QUEEN:
        return attacks_bb<BISHOP>(s, occupied) |
        	   attacks_bb<ROOK>(s, occupied);
    default:
        return PseudoAttacks[pt][s];
    }
}

void bb_init(){
    for(Square s = SQ_A1; s <= SQ_H8; ++s){
        SquareBB[s] = (1ULL << s);
    }
    for(Square s1 = SQ_A1; s1 <= SQ_H8; ++s1){
        for(Square s2 = SQ_A1; s2 <= SQ_H8; ++s2){
            SquareDistance[s1][s2] = max(distance<File>(s1, s2), distance<Rank>(s1, s2));
        }
    }
    for(Square s1 = SQ_A1; s1 <= SQ_H8; ++s1){
        PawnAttacks[WHITE][s1] = pawn_attacks_bb<WHITE>(square_bb(s1));
        PawnAttacks[BLACK][s1] = pawn_attacks_bb<BLACK>(square_bb(s1));
        for(int step: {-9, -8, -7, -1, 1, 7, 8, 9}){
            PseudoAttacks[KING][s1] |= safe_destination(s1, step);
        }
        for(int step: {-17, -15, -10, -6, 6, 10, 15, 17}){
            PseudoAttacks[KNIGHT][s1] |= safe_destination(s1, step);
        }
        PseudoAttacks[QUEEN][s1] = PseudoAttacks[BISHOP][s1] = attacks_bb<BISHOP>(s1, 0);
        PseudoAttacks[QUEEN][s1] |= PseudoAttacks[ROOK][s1] = attacks_bb<ROOK>(s1, 0);
        for(PieceType pt: {BISHOP, ROOK}){
            for(Square s2 = SQ_A1; s2 <= SQ_H8; ++s2){
                if(PseudoAttacks[pt][s1] & s2){
                    LineBB[s1][s2] = (attacks_bb(pt, s1, 0) & attacks_bb(pt, s2, 0)) | s1 | s2;
                    BetweenBB[s1][s2] = (attacks_bb(pt, s1, square_bb(s2)) & attacks_bb(pt, s2, square_bb(s1)));
                }
                BetweenBB[s1][s2] |= s2;
            }
        }
    }
}

BitBoard between_bb(Square s1, Square s2){
    return BetweenBB[s1][s2];
}

BitBoard line_bb(Square s1, Square s2){
    return LineBB[s1][s2];
}

Square lsb(BitBoard b){
    return Square(__builtin_ctzll(b));
}

BitBoard pawn_attacks_bb(Color c, Square s){
    return PawnAttacks[c][s];
}

Square pop_lsb(BitBoard &b){
    const Square s = lsb(b);
    b &= b - 1;
    return s;
}

int popcount(BitBoard b){
    return __builtin_popcountll(b);
}

BitBoard square_bb(Square s){
    return SquareBB[s];
}

BitBoard sliding_attack(PieceType pt, Square sq, BitBoard occupied){
    BitBoard attacks = 0;
    Direction RookDirections[4] = {NORTH, SOUTH, EAST, WEST};
    Direction BishopDirections[4] = {NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
    for(Direction d: (pt == ROOK ? RookDirections : BishopDirections)){
        Square s = sq;
        while(safe_destination(s, d) && !(occupied & s)){
            attacks |= square_bb(s += d);
        }
    }
    return attacks;
}

BitBoard safe_destination(Square s, int step){
    Square to = Square(s + step);
    return is_ok(to) && distance(s, to) <= 2 ? square_bb(to) : BitBoard(0);
}

}
