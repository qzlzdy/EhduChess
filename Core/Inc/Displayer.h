/*
 * Displayer.h
 *
 *  Created on: Mar 11, 2022
 *      Author: qzlzdy
 */

#ifndef INC_DISPLAYER_H_
#define INC_DISPLAYER_H_

#include <Controller.h>
#include <array>
#include <cstdint>

namespace ehdu {

enum Color: uint16_t{
	BLACK = 0x0000,
	BLUE = 0x6E7F,
	GREEN = 0x1C08,
	GREY = 0x8C0E,
	WHITE = 0xFFFF
};

class Displayer {
public:
	virtual void drawBoard(std::array<std::array<Piece, 8>, 8> board) = 0;
	virtual void drawPiece(int x, int y, Piece p) = 0;
	virtual void fill(uint16_t bx, uint16_t by, uint16_t ex,
			  uint16_t ey, Color color) = 0;
};

} /* namespace ehdu */

#endif /* INC_DISPLAYER_H_ */
