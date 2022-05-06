/*
 * Displayer.h
 *
 *  Created on: Mar 11, 2022
 *      Author: qzlzdy
 */

#ifndef INC_DISPLAYER_H_
#define INC_DISPLAYER_H_

#include "types.h"
#include <cstdint>

namespace ehdu {

enum RgbColor: uint16_t{
	THEM = 0x0000,	// BLACK
	BLUE = 0x6E7F,
	GREEN = 0x1C08,
	GREY = 0x8C0E,
	US = 0xFFFF,	// WHITE
	YELLOW = 0xDE87
};

class Displayer {
public:
	Displayer(const Displayer &) = delete;
	Displayer &operator=(const Displayer &) = delete;
	Displayer(Displayer &&) = default;
	Displayer &operator=(Displayer &&) = default;
	virtual ~Displayer() = default;
	virtual void drawBoard(const Piece board[SQUARE_NB]) = 0;
	virtual void drawPiece(int x, int y, Piece p, bool select) = 0;
	virtual void fill(uint16_t bx, uint16_t by, uint16_t ex,
				      uint16_t ey, RgbColor color) = 0;
protected:
	Displayer() = default;
};

} /* namespace ehdu */

#endif /* INC_DISPLAYER_H_ */
