/*
 * Controller.cpp
 *
 *  Created on: 2022年3月12日
 *      Author: qzlzdy
 */

#include "Controller.h"

#include "Ethernet.h"
#include "TftLcd.h"
#include <cctype>

using namespace std;

namespace ehdu {

Controller::Controller() {
	state = CTRL_WAIT;
	TftLcd::getInstance()->drawBoard(board);
}

void Controller::acceptMove(){
	string m;
	if(isalpha(bestmove[13])){
		m = string(bestmove.begin() + 9, bestmove.begin() + 14);
	}
	else{
		m = string(bestmove.begin() + 9, bestmove.begin() + 13);
	}
	do_move(to_move(m));
	TftLcd::getInstance()->drawBoard(board);
	Ethernet::getInstance()->updatePieces(fen() + "\n");
}

void Controller::process(){
	if(state == CTRL_MOVED){
		Ethernet::getInstance()->setPosition("position fen " + fen() + "\n");
		state = CTRL_COMPUTING;
	}
	else if(state == CTRL_FINISHED){
		acceptMove();
		state = CTRL_WAIT;
	}
}

void Controller::select(int x, int y){
	selx = x;
	sely = y;
	TftLcd::getInstance()->drawPiece(x, y, board[y * 8 + x], true);
}

void Controller::touch(int x){
	if(state == CTRL_PROMOTION){
		currmove.append(1, "qrbn"[x]);
		Move m;
		if((m = to_move(currmove)) != MOVE_NONE){
			do_move(m);
			state = CTRL_MOVED;
		}
	}
}

void Controller::touch(int x, int y){
	switch(state){
	case CTRL_WAIT:
		if(color_of(piece_on(make_square(File(x), Rank(y)))) == WHITE){
			select(x, y);
			state = CTRL_SELECTED;
		}
		break;
	case CTRL_SELECTED:
		if(selx == x && sely == y){
			unselect();
			state = CTRL_WAIT;
		}
		else{
			Square from = make_square(File(selx), Rank(sely));
			Square to = make_square(File(x), Rank(y));
			currmove = square(from) + square(to);
			if(piece_on(from) == W_PAWN && rank_of(to) == RANK_8){
				state = CTRL_PROMOTION;
				break;
			}
			Move m;
			if((m = to_move(currmove)) != MOVE_NONE){
				do_move(m);
				state = CTRL_MOVED;
			}
		}
		break;
	default:
		break;
	}
}

void Controller::unselect(){
	TftLcd::getInstance()->drawPiece(selx, sely, board[sely * 8 + selx]);
}

void Controller::setBestmove(const string &m){
	bestmove = m;
	state = CTRL_FINISHED;
}

} /* namespace ehdu */
