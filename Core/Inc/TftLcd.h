/*
 * TftLcd.h
 *
 *  Created on: 2022年3月16日
 *      Author: qzlzdy
 */

#ifndef INC_TFTLCD_H_
#define INC_TFTLCD_H_

#include "Displayer.h"
#include "Singleton.h"
#include "SpiInterface.h"
#include "TouchScreen.h"

#include <map>

namespace ehdu {

class TftLcd final :
public Displayer, public TouchScreen, public SpiInterface,
public Singleton<TftLcd>{
public:
	TftLcd(const TftLcd &) = delete;
	TftLcd &operator=(const TftLcd &) = delete;
	TftLcd(TftLcd &&) = default;
	TftLcd &operator=(TftLcd &&) = default;
	~TftLcd() = default;
	void drawBoard(const std::array<std::array<Piece, 8>, 8> &board) override;
	void drawPiece(int x, int y, Piece p, bool select = false) override;
	void fill(uint16_t bx, uint16_t by, uint16_t ex,
			  uint16_t ey, Color color) override;
private:
	TftLcd();
	void setCursor(uint16_t x, uint16_t y);
	void writeReg(uint8_t reg);
	void writeData(uint16_t data);
	static std::map<Piece, std::array<bool, 900> *> dict;
	friend class Singleton<TftLcd>;
};

} /* namespace ehdu */

#endif /* INC_TFTLCD_H_ */
