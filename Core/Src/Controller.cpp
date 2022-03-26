/*
 * Controller.cpp
 *
 *  Created on: 2022年3月12日
 *      Author: qzlzdy
 */

#include <Controller.h>

#include <TftLcd.h>
#include <sstream>

using namespace std;

namespace ehdu {

Controller::Controller() {
	state = IDLE;
	moves = "moves";
	tpcb = nullptr;
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

	// FIXME debug
	state = MOVED;
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

bool Controller::isBlank(Piece p){
	return p == BLANK;
}

void Controller::process(){
	switch(state){
	case SELECTED:
		break;
	case MOVED:{
		state = BUSY;
		tpcb = tcp_new();
		ctrls[tpcb] = this;
		tcp_sent(tpcb, tcpSent);
		tcp_recv(tpcb, tcpRecv);
		ip_addr_t addr = IPADDR4_INIT_BYTES(169, 254, 82, 59);
		tcp_connect(tpcb, &addr, 2233, tcpConnected);
		break;
	}
	case CONNECTED:{
		state = BUSY;
		string data = toFenString() + " " + moves;
		tcp_write(tpcb, data.c_str(), data.length(), TCP_WRITE_FLAG_COPY);
		tcp_output(tpcb);
		break;
	}
	case SENT:
		state = BUSY;
		break;
	case RECVED:
		ctrls.erase(tpcb);
		tcp_close(tpcb);
		// TODO analyze bestmove and update LCD
		state = IDLE;
		break;
	default:
		break;
	}
}

void Controller::touch(int x, int y){
	if(0 <= x && x < 240 && 40 <= y && y < 280){
		// TODO game logic
	}
}

string Controller::toFenString(){
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

map<struct tcp_pcb *, Controller *> Controller::ctrls;

err_t tcpConnected(void *arg, struct tcp_pcb *tpcb, err_t err){
	tcp_arg(tpcb, arg);
	Controller::ctrls[tpcb]->state = Controller::CONNECTED;
	return err;
}

err_t tcpSent(void *arg, struct tcp_pcb *tpcb, u16_t len){
	tcp_arg(tpcb, arg);
	Controller::ctrls[tpcb]->state = Controller::SENT;
	return ERR_OK;
}

err_t tcpRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err){
	tcp_arg(tpcb, arg);
	Controller::ctrls[tpcb]->state = Controller::RECVED;
	Controller::ctrls[tpcb]->bestmove =
		string(reinterpret_cast<const char *>(p->payload), p->len);
	tcp_recved(tpcb, Controller::ctrls[tpcb]->bestmove.length());
	pbuf_free(p);
	return err;
}

} /* namespace ehdu */
