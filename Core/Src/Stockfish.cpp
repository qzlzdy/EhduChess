#include "Stockfish.h"

#include <cctype>
#include <cstring>
#include <mutex>
#include <sstream>
#include <QGlobalStatic>

#include <iostream>
using namespace std;

namespace{

once_flag BitBoardInit;

}

namespace ehdu{

string Position::move(Move m){
    Square from = from_sq(m);
    Square to = to_sq(m);
    if(m == MOVE_NONE){
        return "(none)";
    }
    if(m == MOVE_NULL){
        return "0000";
    }
    if(type_of(m) == CASTLING){
        to = make_square(to > from ? FILE_G : FILE_C, rank_of(from));
    }
    string move = square(from) + square(to);
    if(type_of(m) == PROMOTION){
        move += " pnbrqk"[promotion_type(m)];
    }
    return move;
}

string Position::square(Square s){
    return string{char('a' + file_of(s)), char('1' + rank_of(s))};
}

Position::Position(){
    call_once(BitBoardInit, bb_init);
    set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");
}

BitBoard Position::blockers_for_king(Color c) const{
    return blockersForKing[c];
}

bool Position::can_castle(CastlingRights cr) const{
    return castlingRights & cr;
}

bool Position::castling_impeded(CastlingRights cr) const{
    return pieces() & castlingPath[cr];
}

Square Position::castling_rook_square(CastlingRights cr) const{
    return castlingRookSquare[cr];
}

BitBoard Position::check_squares(PieceType pt) const{
    return checkSquares[pt];
}

BitBoard Position::checkers() const{
    return checkersBB;
}

Square Position::ep_square() const{
    return epSquare;
}

const string PieceToChar(" PNBRQK  pnbrqk");

string Position::fen() const{
    int emptyCnt;
    ostringstream ss;
    for(Rank r = RANK_8; r >= RANK_1; --r){
        for(File f = FILE_A; f <= FILE_H; ++f){
            for(emptyCnt = 0; f <= FILE_H && empty(make_square(f, r)); ++f){
                ++emptyCnt;
            }
            if(emptyCnt != 0){
                ss << emptyCnt;
            }
            if(f <= FILE_H){
                ss << PieceToChar[piece_on(make_square(f, r))];
            }
        }
        if(r > RANK_1){
            ss << '/';
        }
    }
    return ss.str();
}

bool Position::legal(Move m) const{
    Color us = sideToMove;
    Square from = from_sq(m);
    Square to = to_sq(m);
    if(type_of(m) == EN_PASSANT){
        Square ksq = square<KING>(us);
        Square capsq = to - pawn_push(us);
        BitBoard occupied = (pieces() ^ from ^ capsq) | to;
        return !(attacks_bb<ROOK>(ksq, occupied) & pieces(~us, QUEEN, ROOK)) &&
               !(attacks_bb<BISHOP>(ksq, occupied) & pieces(~us, QUEEN, BISHOP));
    }
    if(type_of(m) == CASTLING){
        to = relative_square(us, to > from ? SQ_G1 : SQ_C1);
        Direction step = to > from ? WEST : EAST;
        for(Square s = to; s != from; s += step){
            if(attackers_to(s) & pieces(~us)){
                return false;
            }
        }
        return !(blockers_for_king(us) & to_sq(m));
    }
    if(type_of(piece_on(from)) == KING){
        return !(attackers_to(to, pieces() ^ from) & pieces(~us));
    }
    return !(blockers_for_king(us) & from) ||
           aligned(from, to, square<KING>(us));
}

BitBoard Position::pieces(PieceType pt) const{
    return byTypeBB[pt];
}

BitBoard Position::pieces(Color c) const{
    return byColorBB[c];
}

BitBoard Position::pieces(Color c, PieceType pt) const{
    return pieces(c) & pieces(pt);
}

Color Position::side_to_move() const{
    return sideToMove;
}

void Position::do_move(Move m){
    do_move(m, gives_check(m));
}

BitBoard Position::attackers_to(Square s) const{
    return attackers_to(s, pieces());
}

BitBoard Position::attackers_to(Square s, BitBoard occupied) const{
    return (pawn_attacks_bb(BLACK, s) & pieces(WHITE, PAWN)) |
           (pawn_attacks_bb(WHITE, s) & pieces(BLACK, PAWN)) |
           (attacks_bb<KNIGHT>(s) & pieces(KNIGHT)) |
           (attacks_bb<ROOK>(s, occupied) & pieces(ROOK, QUEEN)) |
           (attacks_bb<BISHOP>(s, occupied) & pieces(BISHOP, QUEEN)) |
           (attacks_bb<KING>(s) & pieces(KING));
}

void Position:: do_move(Move m, bool givesCheck){
    Color us = sideToMove;
    Color them = ~sideToMove;
    Square from = from_sq(m);
    Square to = to_sq(m);
    Piece pc = piece_on(from);
    Piece captured = type_of(m) == EN_PASSANT ? make_piece(BLACK, PAWN)
                                              : piece_on(to);
    if(type_of(m) == CASTLING){
        Square rfrom, rto;
        do_castling<true>(us, from, to, rfrom, rto);
        captured = NO_PIECE;
    }
    if(captured){
        Square capsq = to;
        if(type_of(captured) == PAWN){
            if(type_of(m) == EN_PASSANT){
                capsq -= pawn_push(us);
            }
        }
        else{

        }
        remove_piece(capsq);
        if(type_of(m) == EN_PASSANT){
            board[capsq] = NO_PIECE;
        }
    }
    if(epSquare != SQ_NONE){
        epSquare = SQ_NONE;
    }
    if(castlingRights && (castlingRightsMask[from] | castlingRightsMask[to])){
        castlingRights &= ~(castlingRightsMask[from] | castlingRightsMask[to]);
    }
    if(type_of(m) != CASTLING){
        move_piece(from, to);
    }
    if(type_of(pc) == PAWN){
        if((int(to) ^ int(from)) == 16 &&
           (pawn_attacks_bb(us, to - pawn_push(us)) & pieces(them, PAWN))){
            epSquare = to - pawn_push(us);
        }
        else if(type_of(m) == PROMOTION){
            Piece promotion = make_piece(us, promotion_type(m));
            remove_piece(to);
            put_piece(promotion, to);
        }
    }
    checkersBB = givesCheck ? attackers_to(square<KING>(them)) & pieces(us) : 0;
    sideToMove = ~sideToMove;
    set_check_info();
}

Piece Position::piece_on(Square s) const{
    return board[s];
}

void Position::set(const string &fenStr){
    unsigned char token;
    size_t idx;
    Square sq = SQ_A8;
    istringstream ss(fenStr);
    memset(static_cast<void *>(this), 0, sizeof(Position));
    ss >> noskipws;
    while((ss >> token) && !isspace(token)){
        if(isdigit(token)){
            sq += (token - '0') * EAST;
        }
        else if(token == '/'){
            sq += 2 * SOUTH;
        }
        else if((idx = PieceToChar.find(token)) != string::npos){
            put_piece(Piece(idx), sq);
            ++sq;
        }
    }
    ss >> token;
    sideToMove = (token == 'w' ? WHITE : BLACK);
    ss >> token;
    while((ss >> token) && !isspace(token)){
        Square rsq;
        Color c = islower(token) ? BLACK : WHITE;
        Piece rook = make_piece(c, ROOK);
        token = char(toupper(token));
        if(token == 'K'){
            for(rsq = relative_square(c, SQ_H1); piece_on(rsq) != rook; --rsq);
        }
        else if(token == 'Q'){
            for(rsq = relative_square(c, SQ_A1); piece_on(rsq) != rook; ++rsq);
        }
        else if('A' <= token && token <= 'H'){
            rsq = make_square(File(token - 'A'), relative_rank(c, RANK_1));
        }
        else{
            continue;
        }
        set_castling_right(c, rsq);
    }
    epSquare = SQ_NONE;
    checkersBB = attackers_to(square<KING>(sideToMove)) & pieces(~sideToMove);
}

namespace{

template<GenType Type, Direction D>
Move *make_promotions(Move *moveList, Square to){
    if(Type == CAPTURES || Type == EVASIONS || Type == NON_EVASIONS){
        *moveList++ = make<PROMOTION>(to - D, to, QUEEN);
    }
    if(Type == QUIETS || Type == EVASIONS || Type == NON_EVASIONS){
        *moveList++ = make<PROMOTION>(to - D, to, ROOK);
        *moveList++ = make<PROMOTION>(to - D, to, BISHOP);
        *moveList++ = make<PROMOTION>(to - D, to, KNIGHT);
    }
    return moveList;
}

template<Color Us, GenType Type>
Move *generate_pawn_moves(const Position &pos, Move *moveList, BitBoard target){
    constexpr Color Them = ~Us;
    constexpr BitBoard TRank7BB = (Us == WHITE ? Rank7BB : Rank2BB);
    constexpr BitBoard TRank3BB = (Us == WHITE ? Rank3BB : Rank6BB);
    constexpr Direction Up = pawn_push(Us);
    constexpr Direction UpRight = (Us == WHITE ? NORTH_EAST : SOUTH_WEST);
    constexpr Direction UpLeft = (Us == WHITE ? NORTH_WEST : SOUTH_EAST);
    const BitBoard emptySquares = ~pos.pieces();
    const BitBoard enemies = Type == EVASIONS ? pos.checkers() : pos.pieces(Them);
    BitBoard pawnsOn7 = pos.pieces(Us, PAWN) & TRank7BB;
    BitBoard pawnsNotOn7 = pos.pieces(Us, PAWN) & ~TRank7BB;
    if(Type != CAPTURES){
        BitBoard b1 = shift<Up>(pawnsNotOn7) & emptySquares;
        BitBoard b2 = shift<Up>(b1 & TRank3BB) & emptySquares;
        if(Type == EVASIONS){
            b1 &= target;
            b2 &= target;
        }
        if(Type == QUIET_CHECKS){
            Square ksq = pos.square<KING>(Them);
            BitBoard dcCandidatePawns = pos.blockers_for_king(Them) & ~file_bb(ksq);
            b1 &= pawn_attacks_bb(Them, ksq) | shift<Up>(dcCandidatePawns);
            b2 &= pawn_attacks_bb(Them, ksq) | shift<Up + Up>(dcCandidatePawns);
        }
        while(b1){
            Square to = pop_lsb(b1);
            *moveList++ = make_move(to - Up, to);
        }
        while(b2){
            Square to = pop_lsb(b2);
            *moveList++ = make_move(to - Up - Up, to);
        }
    }
    if(pawnsOn7){
        BitBoard b1 = shift<UpRight>(pawnsOn7) & enemies;
        BitBoard b2 = shift<UpLeft>(pawnsOn7) & enemies;
        BitBoard b3 = shift<Up>(pawnsOn7) & emptySquares;
        if(Type == EVASIONS){
            b3 &= target;
        }
        while(b1){
            moveList = make_promotions<Type, UpRight>(moveList, pop_lsb(b1));
        }
        while(b2){
            moveList = make_promotions<Type, UpLeft>(moveList, pop_lsb(b2));
        }
        while(b3){
            moveList = make_promotions<Type, Up>(moveList, pop_lsb(b3));
        }
    }
    if(Type == CAPTURES || Type == EVASIONS || Type == NON_EVASIONS){
        BitBoard b1 = shift<UpRight>(pawnsNotOn7) & enemies;
        BitBoard b2 = shift<UpLeft>(pawnsNotOn7) & enemies;
        while(b1){
            Square to = pop_lsb(b1);
            *moveList++ = make_move(to - UpRight, to);
        }
        while(b2){
            Square to = pop_lsb(b2);
            *moveList++ = make_move(to - UpLeft, to);
        }
        if(pos.ep_square() != SQ_NONE){
            if(Type == EVASIONS && (target & (pos.ep_square() + Up))){
                return moveList;
            }
            b1 = pawnsNotOn7 & pawn_attacks_bb(Them, pos.ep_square());
            while(b1){
                *moveList++ = make<EN_PASSANT>(pop_lsb(b1), pos.ep_square());
            }
        }
    }
    return moveList;
}

template<Color Us, PieceType Pt, bool Checks>
Move *generate_moves(const Position &pos, Move *moveList, BitBoard target){
    BitBoard bb = pos.pieces(Us, Pt);
    while(bb){
        Square from = pop_lsb(bb);
        BitBoard b = attacks_bb<Pt>(from, pos.pieces()) & target;
        if(Checks && (Pt == QUEEN || !(pos.blockers_for_king(~Us) & from))){
            b &= pos.check_squares(Pt);
        }
        while(b){
            *moveList++ = make_move(from, pop_lsb(b));
        }
    }
    return moveList;
}

template<Color Us, GenType Type>
Move *generate_all(const Position &pos, Move *moveList){
    constexpr bool Checks = Type == QUIET_CHECKS;
    const Square ksq = pos.square<KING>(Us);
    BitBoard target;
    if(Type != EVASIONS || !more_than_one(pos.checkers())){
        target = Type == EVASIONS ? between_bb(ksq, lsb(pos.checkers()))
               : Type == NON_EVASIONS ? ~pos.pieces(Us)
               : Type == CAPTURES ? pos.pieces(~Us)
                                  : ~pos.pieces();
        moveList = generate_pawn_moves<Us, Type>(pos, moveList, target);
        moveList = generate_moves<Us, KNIGHT, Checks>(pos, moveList, target);
        moveList = generate_moves<Us, BISHOP, Checks>(pos, moveList, target);
        moveList = generate_moves<Us, ROOK, Checks>(pos, moveList, target);
        moveList = generate_moves<Us, QUEEN, Checks>(pos, moveList, target);
    }
    if(!Checks || pos.blockers_for_king(~Us) & ksq){
        BitBoard b = attacks_bb<KING>(ksq) & (Type == EVASIONS ? ~pos.pieces(Us) : target);
        if(Checks){
            b &= ~attacks_bb<QUEEN>(pos.square<KING>(~Us));
        }
        while(b){
            *moveList++ = make_move(ksq, pop_lsb(b));
        }
        if((Type == QUIETS || Type == NON_EVASIONS) && pos.can_castle(Us & ANY_CASTLING)){
            for(CastlingRights cr : {Us & KING_SIDE, Us & QUEEN_SIDE}){
                if(!pos.castling_impeded(cr) && pos.can_castle(cr)){
                    *moveList++ = make<CASTLING>(ksq, pos.castling_rook_square(cr));
                }
            }
        }
    }
    return moveList;
}

template<GenType Type>
Move *generate(const Position &pos, Move *moveList){
    Color us = pos.side_to_move();
    return us == WHITE ? generate_all<WHITE, Type>(pos, moveList)
                       : generate_all<BLACK, Type>(pos, moveList);
}

template<>
Move *generate<LEGAL>(const Position &pos, Move *moveList){
    Color us = pos.side_to_move();
    BitBoard pinned = pos.blockers_for_king(us) & pos.pieces(us);
    Square ksq = pos.square<KING>(us);
    Move *cur = moveList;
    moveList = pos.checkers() ? generate<EVASIONS>(pos, moveList)
                              : generate<NON_EVASIONS>(pos, moveList);
    while(cur != moveList){
        if(((pinned && pinned & from_sq(*cur)) || from_sq(*cur) == ksq || type_of(*cur) == EN_PASSANT) &&
           !pos.legal(*cur)){
            *cur = *(--moveList);
        }
        else{
            ++cur;
        }
    }
    return moveList;
}

template<GenType T>
class MoveList{
public:
    explicit MoveList(const Position &pos): last(generate<T>(pos, moveLists)){

    }
    const Move *begin() const{
        return moveLists;
    }
    const Move *end() const{
        return last;
    }
private:
    Move moveLists[MAX_MOVES];
    Move *last;
};

}

Move Position::to_move(string &str){
    for(const auto &m : MoveList<LEGAL>(*this)){
        if(str == move(m)){
            return m;
        }
    }
    return MOVE_NONE;
}

bool Position::empty(Square s) const{
    return piece_on(s) == NO_PIECE;
}

bool Position::gives_check(Move m) const{
    Square from = from_sq(m);
    Square to = to_sq(m);
    if(check_squares(type_of(piece_on(from))) & to){
        return true;
    }
    if((blockers_for_king(BLACK) & from) &&
       !aligned(from, to, square<KING>(BLACK))){
        return false;
    }
    switch(type_of(m)){
    case NORMAL:
        return false;
    case PROMOTION:
        return attacks_bb(promotion_type(m), to, pieces() ^ from) &
               square<KING>(BLACK);
    case EN_PASSANT:
    {
        Square capsq = make_square(file_of(to), rank_of(from));
        BitBoard b = (pieces() ^ from ^ capsq) | to;
        return (attacks_bb<ROOK>(square<KING>(BLACK), b) &
                pieces(WHITE, QUEEN, ROOK)) |
               (attacks_bb<BISHOP>(square<KING>(BLACK), b) &
                pieces(WHITE, QUEEN, BISHOP));
    }
    default:
    {
        Square ksq = square<KING>(BLACK);
        Square rto = relative_square(WHITE, to > from ? SQ_F1 : SQ_D1);
        return (attacks_bb<ROOK>(rto) & ksq) &&
               (attacks_bb<ROOK>(rto, pieces() ^ from ^ to) & ksq);
    }
    }
}

void Position::move_piece(Square from, Square to){
    Piece pc = board[from];
    BitBoard fromTo = from | to;
    byTypeBB[ALL_PIECES] ^= fromTo;
    byTypeBB[type_of(pc)] ^= fromTo;
    byColorBB[color_of(pc)] ^= fromTo;
    board[from] = NO_PIECE;
    board[to] = pc;
}

Piece Position::moved_piece(Move m) const{
    return piece_on(from_sq(m));
}

void Position::put_piece(Piece pc, Square s){
    board[s] = pc;
    byTypeBB[ALL_PIECES] |= byTypeBB[type_of(pc)] |= s;
    byColorBB[color_of(pc)] |= s;
}

void Position::remove_piece(Square s){
    Piece pc = board[s];
    byTypeBB[ALL_PIECES] ^= s;
    byTypeBB[type_of(pc)] ^= s;
    byColorBB[color_of(pc)] ^= s;
    board[s] = NO_PIECE;
}

void Position::set_castling_right(Color c, Square rfrom){
    Square kfrom = square<KING>(c);
    CastlingRights cr = c & (kfrom < rfrom ? KING_SIDE : QUEEN_SIDE);
    castlingRights |= cr;
    castlingRightsMask[kfrom] |= cr;
    castlingRightsMask[rfrom] |= cr;
    castlingRookSquare[cr] = rfrom;
    Square kto = relative_square(c, cr & KING_SIDE ? SQ_G1 : SQ_C1);
    Square rto = relative_square(c, cr & KING_SIDE ? SQ_F1 : SQ_D1);
    castlingPath[cr] = (between_bb(rfrom, rto) | between_bb(kfrom, kto)) &
                        ~(kfrom | rfrom);
}

void Position::set_check_info(){
    blockersForKing[WHITE] =
            slider_blockers(pieces(BLACK), square<KING>(WHITE), pinners[BLACK]);
    blockersForKing[BLACK] =
            slider_blockers(pieces(WHITE), square<KING>(BLACK), pinners[WHITE]);
    Square ksq = square<KING>(~sideToMove);
    checkSquares[PAWN] = pawn_attacks_bb(~sideToMove, ksq);
    checkSquares[KNIGHT] = attacks_bb<KNIGHT>(ksq);
    checkSquares[BISHOP] = attacks_bb<BISHOP>(ksq, pieces());
    checkSquares[ROOK] = attacks_bb<ROOK>(ksq, pieces());
    checkSquares[QUEEN] = checkSquares[BISHOP] | checkSquares[ROOK];
    checkSquares[KING] = 0;
}

BitBoard Position::slider_blockers(BitBoard sliders, Square s,
                                   BitBoard &pinners) const{
    BitBoard blockers = 0;
    pinners = 0;
    BitBoard snipers = ((attacks_bb<ROOK>(s) & pieces(QUEEN, ROOK)) |
                        (attacks_bb<BISHOP>(s) & pieces(QUEEN, BISHOP))) & sliders;
    BitBoard occupancy = pieces() ^ snipers;
    while(snipers){
        Square sniperSq = pop_lsb(snipers);
        BitBoard b = between_bb(s, sniperSq) & occupancy;
        if(b && !more_than_one(b)){
            blockers |= b;
            if(b & pieces(color_of(piece_on(s)))){
                pinners |= sniperSq;
            }
        }
    }
    return blockers;
}

BitBoard Position::pieces(PieceType pt1, PieceType pt2) const{
    return pieces(pt1) | pieces(pt2);
}

BitBoard Position::pieces(Color c, PieceType pt1, PieceType pt2) const{
    return pieces(c) & (pieces(pt1) | pieces(pt2));
}

}
