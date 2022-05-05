#include "BitBoard.h"

#include <bitset>
#include <initializer_list>
#include <intrin.h>

#include <iostream>
using namespace std;

namespace ehdu{

BitBoard PawnAttacks[COLOR_NB][SQUARE_NB];
BitBoard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];

Magic RookMagics[SQUARE_NB];
Magic BishopMagics[SQUARE_NB];

uint8_t PopCnt16[1 << 16];
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

unsigned Magic::index(BitBoard occupied) const{
    unsigned lo = unsigned(occupied) & unsigned(mask);
    unsigned hi = unsigned(occupied >> 32) & unsigned(mask >> 32);
    return (lo * unsigned(magic) ^ hi * unsigned(magic >> 32)) >> shift;
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
        return attacks_bb<BISHOP>(s, occupied) | attacks_bb<ROOK>(s, occupied);
    default:
        return PseudoAttacks[pt][s];
    }
}

namespace {

BitBoard RookTable[0x19000];
BitBoard BishopTable[0x1480];

class PRNG{
public:
    PRNG(uint64_t seed): s(seed){}
    template<class T>
    T sparse_rand(){
        return T(rand64() & rand64() & rand64());
    }
private:
    uint64_t rand64(){
        s ^= s >> 12;
        s ^= s << 25;
        s ^= s >> 27;
        return s * 2685821657736338717LL;
    }
    uint64_t s;
};

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

void init_magics(PieceType pt, BitBoard table[], Magic magics[]){
    int seeds[RANK_NB] = {8977, 44560, 54343, 38998, 5731, 95205, 104912, 17020};
    BitBoard occupancy[4096], reference[4096], edges, b;
    int epoch[4096] = {}, cnt = 0, size = 0;
    for(Square s = SQ_A1; s <= SQ_H8; ++s){
        edges = ((Rank1BB | Rank8BB) & ~rank_bb(s)) | ((FileABB | FileHBB) & ~file_bb(s));
        Magic &m = magics[s];
        m.mask = sliding_attack(pt, s, 0) & ~edges;
        m.shift = 32 - popcount(m.mask);
        m.attacks = s == SQ_A1 ? table : magics[s - 1].attacks + size;
        b = size = 0;
        do{
            occupancy[size] = b;
            reference[size] = sliding_attack(pt, s, b);
            ++size;
            b = (b - m.mask) & m.mask;
        }while(b);
        PRNG rng(seeds[rank_of(s)]);
        for(int i = 0; i < size; ){
            for(m.magic = 0; popcount((m.magic * m.mask) >> 56) < 6; ){
                m.magic = rng.sparse_rand<BitBoard>();
            }
            for(++cnt, i = 0; i < size; ++i){
                unsigned idx = m.index(occupancy[i]);
                if(epoch[idx] < cnt){
                    epoch[idx] = cnt;
                    m.attacks[idx] = reference[i];
                }
                else if(m.attacks[idx] != reference[i]){
                    break;
                }
            }
        }
    }
}

}

void bb_init(){
    for(unsigned i = 0; i < (1 << 16); ++i){
        PopCnt16[i] = uint8_t(bitset<16>(i).count());   // FIXME DELETE
    }
    for(Square s = SQ_A1; s <= SQ_H8; ++s){
        SquareBB[s] = (1ULL << s);
    }
    for(Square s1 = SQ_A1; s1 <= SQ_H8; ++s1){
        for(Square s2 = SQ_A1; s2 <= SQ_H8; ++s2){
            SquareDistance[s1][s2] = max(distance<File>(s1, s2), distance<Rank>(s1, s2));
        }
    }
    init_magics(ROOK, RookTable, RookMagics);
    init_magics(BISHOP, BishopTable, BishopMagics);
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

//Square lsb(BitBoard b){
//    return Square(__builtin_ctzll(b));
//}

Square lsb(BitBoard b){
    unsigned long idx;
    _BitScanForward64(&idx, b);
    return static_cast<Square>(idx);
}

BitBoard pawn_attacks_bb(Color c, Square s){
    return PawnAttacks[c][s];
}

Square pop_lsb(BitBoard &b){
    const Square s = lsb(b);
    b &= b - 1;
    return s;
}

//int popcount(BitBoard b){
//    return __builtin_popcountll(b);
//}

int popcount(BitBoard b){
    union{
        BitBoard bb;
        uint16_t u[4];
    } v = {b};
    return PopCnt16[v.u[0]] + PopCnt16[v.u[1]] + PopCnt16[v.u[2]] + PopCnt16[v.u[3]];
}

BitBoard square_bb(Square s){
    return SquareBB[s];
}

BitBoard safe_destination(Square s, int step){
    Square to = Square(s + step);
    return is_ok(to) && distance(s, to) <= 2 ? square_bb(to) : BitBoard(0);
}

}
