/*
 * Controller.h
 *
 *  Created on: 2022年3月12日
 *      Author: qzlzdy
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include "ControllerInterface.h"
#include "LoopInterface.h"

#include <array>
#include <cstdint>
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

class Controller: public ControllerInterface, public LoopInterface{
public:
	Controller();
	Controller(const Controller &) = default;
	Controller &operator=(const Controller &) = default;
	Controller(Controller &&) = default;
	Controller &operator=(Controller &&) = default;
	~Controller() = default;
	static bool isWhite(Piece p);
	static bool isBlack(Piece p);
	static bool isBlank(Piece p);
	void acceptMove();
	bool isLegal(int ex, int ey) const;
	bool isPawnLegal(int ex, int ey) const;
	bool isRookLegal(int ex, int ey) const;
	bool isKnightLegal(int ex, int ey) const;
	bool isBishopLegal(int ex, int ey) const;
	bool isQueenLegal(int ex, int ey) const;
	bool isKingLegal(int ex, int ey) const;
	void move(int x, int y);
	void process() override;
	void select(int x, int y);
	void setBestmove(const std::string &m) override;
	void touch(int x, int y) override;
	void unselect();
private:
	enum CtrlState{
		CTRL_WAIT,
		CTRL_SELECTED,
		CTRL_MOVED,
		CTRL_COMPUTING,
		CTRL_FINISHED
	};
	bool checkBlank(int dx, int dy, int ex, int ey) const;
	std::string toFenString() const;
	std::array<std::array<Piece, 8>, 8> board;
	std::string bestmove;
	std::string moves;
	CtrlState state;
	int selx, sely;
};

} /* namespace ehdu */

#endif /* INC_CONTROLLER_H_ */
