/*
 * Controller.cpp
 *
 *  Created on: 2022年3月12日
 *      Author: qzlzdy
 */

#include "Controller.h"

#include "Ethernet.h"
#include "TftLcd.h"

#include <algorithm>
#include <cmath>
#include <sstream>

using namespace std;

namespace ehdu {

Controller::Controller() {
	state = CTRL_WAIT;
	moves = "moves";
	for(int y = 2; y <= 5; ++y){
		for(int x = 0; x < 8; ++x){
			board[x][y] = BLANK;
		}
	}
	for(int x = 0; x < 8; ++x){
		board[x][1] = WHITE_PAWN;
		board[x][6] = BLACK_PAWN;
	}
	board[0][0] = board[7][0] = WHITE_ROOK;
	board[0][7] = board[7][7] = BLACK_ROOK;
	board[1][0] = board[6][0] = WHITE_KNIGHT;
	board[1][7] = board[6][7] = BLACK_KNIGHT;
	board[2][0] = board[5][0] = WHITE_BISHOP;
	board[2][7] = board[5][7] = BLACK_BISHOP;
	board[3][0] = WHITE_QUEEN;
	board[3][7] = BLACK_QUEEN;
	board[4][0] = WHITE_KING;
	board[4][7] = BLACK_KING;
	TftLcd::getInstance()->drawBoard(board);
}

bool Controller::isWhite(Piece p){
	switch(p){
	case WHITE_PAWN:
	case WHITE_ROOK:
	case WHITE_KNIGHT:
	case WHITE_BISHOP:
	case WHITE_QUEEN:
	case WHITE_KING:
		return true;
	default:
		return false;
	}
}

bool Controller::isBlack(Piece p){
	switch(p){
	case BLACK_PAWN:
	case BLACK_ROOK:
	case BLACK_KNIGHT:
	case BLACK_BISHOP:
	case BLACK_QUEEN:
	case BLACK_KING:
		return true;
	default:
		return false;
	}
}

bool Controller::isBlank(Piece p){
	return p == BLANK;
}

void Controller::acceptMove(){
	int bx = bestmove[0] - 'a';
	int by = bestmove[1] - '0';
	int ex = bestmove[2] - 'a';
	int ey = bestmove[3] - '0';
	board[ex][ey] = board[bx][by];
	board[bx][by] = BLANK;
	TftLcd::getInstance()->drawPiece(bx, by, BLANK);
	TftLcd::getInstance()->drawPiece(ex, ey, board[ex][ey]);
}

bool Controller::isLegal(int ex, int ey) const{
	switch(board[selx][sely]){
	case WHITE_PAWN:
		return isPawnLegal(ex, ey);
	case WHITE_ROOK:
		return isRookLegal(ex, ey);
	case WHITE_KNIGHT:
		return isKnightLegal(ex, ey);
	case WHITE_BISHOP:
		return isBishopLegal(ex, ey);
	case WHITE_QUEEN:
	 	return isQueenLegal(ex, ey);
	case WHITE_KING:
		return isKingLegal(ex, ey);
	default:
		return false;
	}
}

bool Controller::isPawnLegal(int ex, int ey) const{
	if(ey - sely != 1){
		return false;
	}
	switch(ex - selx){
	case 0:
		return isBlank(board[ex][ey]);
	case 1:
	case -1:
		return isBlack(board[ex][ey]);
	default:
		return false;
	}
}

bool Controller::isRookLegal(int ex, int ey) const{
	int dx = ex == selx ? 0 : ex > selx ? 1 : -1;
	int dy = ey == sely ? 0 : ey > sely ? 1 : -1;
	if(dx != 0 && dy != 0){
		return false;
	}
	return checkBlank(dx, dy, ex, ey);
}

bool Controller::isKnightLegal(int ex, int ey) const{
	return (abs(ex - selx) == 1 && abs(ey - sely) == 2) ||
		   (abs(ex - selx) == 2 && abs(ey - sely) == 1);
}

bool Controller::isBishopLegal(int ex, int ey) const{
	if(abs(ex - selx) != abs(ey - sely)){
		return false;
	}
	int dx = ex > selx ? 1 : -1;
	int dy = ey > sely ? 1 : -1;
	return checkBlank(dx, dy, ex, ey);
}

bool Controller::isQueenLegal(int ex, int ey) const{
	return isRookLegal(ex, ey) || isBishopLegal(ex, ey);
}

bool Controller::isKingLegal(int ex, int ey) const{
	return abs(ex - selx) + abs(ey - sely) <= 2;
}

void Controller::move(int ex, int ey){
	TftLcd::getInstance()->drawPiece(selx, sely, BLANK);
	TftLcd::getInstance()->drawPiece(ex, ey, board[selx][sely]);
	board[ex][ey] = board[selx][sely];
	board[selx][sely] = BLANK;
	moves += " " + "abcdefgh"[selx] + "12345678"[sely]
			     + "abcdefgh"[ex] + "12345678"[ey];
}

void Controller::process(){
	if(state == CTRL_MOVED){
		Ethernet::getInstance()->setPosition(toFenString() + " " + moves);
		state = CTRL_COMPUTING;
	}
}

void Controller::select(int x, int y){
	selx = x;
	sely = y;
	TftLcd::getInstance()->drawPiece(x, y, board[x][y], true);
}

void Controller::setBestmove(const string &m){
	bestmove = m;
	state = CTRL_FINISHED;
}

void Controller::touch(int x, int y){
	switch(state){
	case CTRL_WAIT:
		if(isWhite(board[x][y])){
			select(x, y);
			state = CTRL_SELECTED;
		}
		break;
	case CTRL_SELECTED:
		if(selx == x && sely == y){
			unselect();
			state = CTRL_WAIT;
		}
		else if(!isWhite(board[x][y]) && isLegal(x, y)){
			move(x, y);
			state = CTRL_MOVED;
		}
		break;
	case CTRL_FINISHED:
		acceptMove();
		state = CTRL_WAIT;
	default:
		break;
	}
}

void Controller::unselect(){
	TftLcd::getInstance()->drawPiece(selx, sely, board[selx][sely]);
}

bool Controller::checkBlank(int dx, int dy, int ex, int ey) const{
	int i = selx + dx;
	int j = sely + dy;
	bool canMove = true;
	while(i != ex || j != ey){
		if(isBlank(board[i][j])){
			canMove = false;
			break;
		}
		i += dx;
		j += dy;
	}
	return canMove;
}

string Controller::toFenString() const{
	ostringstream buffer;
	for(int y = 7; y >= 0; --y){
		int blank = 0;
		for(int x = 0; x < 8; ++x){
			if(!isBlank(board[x][y])){
				if(blank != 0){
					buffer << blank;
					blank = 0;
				}
				buffer << static_cast<char>(board[x][y]);
			}
			else{
				++blank;
			}
		}
		if(blank != 0){
			buffer << blank;
		}
		if(y > 0){
			buffer << '/';
		}
	}
	return "fen " + buffer.str();
}

} /* namespace ehdu */
