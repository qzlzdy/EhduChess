/*
 * Controller.h
 *
 *  Created on: 2022年3月12日
 *      Author: qzlzdy
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include <lwip.h>
#include <array>
#include <cstdint>
#include <map>
#include <string>

namespace ehdu {

enum Piece: uint8_t{
	BLACK_PAWN = 'P',
	BLACK_ROOK = 'R',
	BLACK_KNIGHT = 'N',
	BLACK_BISHOP = 'B',
	BLACK_QUEEN = 'Q',
	BLACK_KING = 'K',
	WHITE_PAWN = 'p',
	WHITE_ROOK = 'r',
	WHITE_KNIGHT = 'n',
	WHITE_BISHOP = 'b',
	WHITE_QUEEN = 'q',
	WHITE_KING = 'k',
	BLANK = ' '
};

class Controller {
public:
	Controller();
	static bool isWhite(Piece p);
	static bool isBlank(Piece p);
	void process();
	void touch(int x, int y);
private:
	enum State{
		IDLE,
		SELECTED,
		MOVED,
		CONNECTED,
		SENT,
		RECVED,
		BUSY
	};
	std::string toFenString();
	State state;
	std::array<std::array<Piece, 8>, 8> board;
	std::string moves;
	std::string bestmove;
	struct tcp_pcb *tpcb;
	static std::map<struct tcp_pcb *, Controller *> ctrls;
	friend err_t tcpConnected(void *arg, struct tcp_pcb *tpcb, err_t err);
	friend err_t tcpSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
	friend err_t tcpRecv(void *arg, struct tcp_pcb *tpcb,
						 struct pbuf *p, err_t err);
};


err_t tcpConnected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t tcpSent(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t tcpRecv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

} /* namespace ehdu */

#endif /* INC_CONTROLLER_H_ */
