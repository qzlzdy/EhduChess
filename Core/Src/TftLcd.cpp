/*
 * TftLcd.cpp
 *
 *  Created on: Mar 11, 2022
 *      Author: qzlzdy
 */

#include "TftLcd.h"

#include "main.h"
#include <utility>

using namespace std;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	uint8_t txData[] = {0xD0, 0, 0x90, 0, 0};
	uint8_t rxData[5];
	auto hspi = ehdu::TftLcd::getInstance()->getSpi();
	HAL_SPI_TransmitReceive(hspi, txData, rxData, 5, 1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
	unsigned x = (rxData[1] << 5) | (rxData[2] >> 3);
	unsigned y = (rxData[3] << 5) | (rxData[4] >> 3);
	x = 240 - x / 15.833333;
	y = y / 12.5;
	if(y < 35){
		ehdu::TftLcd::getInstance()->getController()->touch(x);
	}
	else{
		ehdu::TftLcd::getInstance()->getController()->touch(x, y);
	}
}

namespace{

array<uint32_t, 30> blank = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

array<uint32_t, 30> pawn = {
	0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x1e000, 0x3f000, 0x7f800, 0xffc00,
	0xffc00, 0xffc00, 0xffc00, 0x7f800, 0x7f800,
	0xffc00, 0xffc00, 0x7f800, 0x3f000, 0x7f800,
	0xffc00, 0x1ffe00, 0x3fff00, 0x7fff80, 0x7fff80,
	0x7fff80, 0x7fff80, 0x7fff80, 0x0, 0x0
};

array<uint32_t, 30> rook = {
	0x0, 0x0, 0x0, 0x0, 0x0,
	0x38c700, 0x7def80, 0x7fff80, 0x7fff80, 0x3fff00,
	0x3fff00, 0x3fff00, 0x1ffe00, 0xffc00, 0xffc00,
	0x1ffe00, 0x1ffe00, 0x1ffe00, 0x1ffe00, 0x1ffe00,
	0x1ffe00, 0x3fff00, 0x7fff80, 0xffffc0, 0xffffc0,
	0xffffc0, 0xffffc0, 0x0, 0x0, 0x0
};

array<uint32_t, 30> knight = {
	0x0, 0x0, 0x0, 0x60000, 0xf0000,
	0xfe000, 0x1ff800, 0x3ffc00, 0x3ffe00, 0x7fff00,
	0x7fff80, 0xffffc0, 0xffffc0, 0x1ffffc0, 0x3ffffc0,
	0x3ffffc0, 0x3f1ffc0, 0xe3ffc0, 0x7ffc0, 0xfffc0,
	0x1fff80, 0x1fff80, 0x3fffc0, 0x7fffe0, 0x7fffe0,
	0x7fffe0, 0x7fffe0, 0x0, 0x0, 0x0
};

array<uint32_t, 30> bishop = {
	0x0, 0x0, 0x0, 0xc000, 0x1c000,
	0x1c000, 0x38000, 0x38000, 0x70000, 0xf1800,
	0xf1c00, 0x1f1c00, 0x1f3e00, 0x3e3e00, 0x3e3e00,
	0x3f3e00, 0x3ffe00, 0x3ffe00, 0x1ffe00, 0x1ffc00,
	0xffc00, 0x7f800, 0x3ffe00, 0x7fff80, 0x7fff80,
	0x7fff80, 0x0, 0x0, 0x0, 0x0
};

array<uint32_t, 30> queen = {
	0x0, 0x0, 0x0, 0xe1c00, 0x1f3e00,
	0x1f3e00, 0x1f3e00, 0x1f3e00, 0x3ce1cf0, 0x7cf3cf8,
	0x7effdf8, 0x7effdf8, 0x3effdf0, 0x1ffffe0, 0xffffc0,
	0xffffc0, 0x7fff80, 0x7fff80, 0x7fff80, 0x3fff00,
	0x3fff00, 0x1ffe00, 0xffffc0, 0x1ffffe0, 0x1ffffe0,
	0x1ffffe0, 0x1ffffe0, 0x0, 0x0, 0x0
};

array<uint32_t, 30> king = {
	0x0, 0x0, 0x0, 0xc000, 0xc000,
	0x3f000, 0x3f000, 0xc000, 0xc000, 0x7def80,
	0x1ffffe0, 0x1ffffe0, 0x3fffff0, 0x3e3f1f0, 0x3c1e0f0,
	0x3e1e1f0, 0x3e1e1f0, 0x1f1e3e0, 0xffffc0, 0x7fff80,
	0x3fff00, 0x1c0e00, 0x3fff00, 0x7fff80, 0x7fff80,
	0x7fff80, 0x0, 0x0, 0x0, 0x0
};

}

namespace ehdu {

void TftLcd::drawBoard(const Piece board[]){
	for(int y = 0; y < 8; ++y){
		for(int x = 0; x < 8; ++x){
			drawPiece(x, y, board[y * 8 + x]);
		}
	}
}

void TftLcd::drawPiece(int x, int y, Piece p, bool select){
	RgbColor front = color_of(p) == WHITE ? US : THEM;
	RgbColor back = select ? YELLOW : ((x + y) % 2 == 0 ? GREEN : GREY);
	array<uint32_t, 30> &pixels = *dict[p];
	for(int h = 0; h < 30; ++h){
		setCursor(x * 30, y * 30 + 69 - h);
		writeReg(0x2C);
		for(int w = 0; w < 30; ++w){
			writeData(pixels[h] & (1 << (29 - w)) ? front : back);
		}
	}
}

void TftLcd::fill(uint16_t bx, uint16_t by, uint16_t ex,
		          uint16_t ey, RgbColor color){
	for(int i = by; i <= ey; ++i){
		setCursor(bx, i);
		writeReg(0x2C);
		for(int j = bx; j <= ex; ++j){
			writeData(color);
		}
	}
}

TftLcd::TftLcd() {
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(100);
	writeReg(0xCF);
	writeData(0x00);
	writeData(0xC1);
	writeData(0x30);
	writeReg(0xED);
	writeData(0x64);
	writeData(0x03);
	writeData(0X12);
	writeData(0X81);
	writeReg(0xE8);
	writeData(0x85);
	writeData(0x10);
	writeData(0x7A);
	writeReg(0xCB);
	writeData(0x39);
	writeData(0x2C);
	writeData(0x00);
	writeData(0x34);
	writeData(0x02);
	writeReg(0xF7);
	writeData(0x20);
	writeReg(0xEA);
	writeData(0x00);
	writeData(0x00);
	writeReg(0xC0);
	writeData(0x1B);
	writeReg(0xC1);
	writeData(0x01);
	writeReg(0xC5);
	writeData(0x30);
	writeData(0x30);
	writeReg(0xC7);
	writeData(0XB7);
	writeReg(0x36);
	writeData(0x48);
	writeReg(0x3A);
	writeData(0x55);
	writeReg(0xB1);
	writeData(0x00);
	writeData(0x1A);
	writeReg(0xB6);
	writeData(0x0A);
	writeData(0xA2);
	writeReg(0xF2);
	writeData(0x00);
	writeReg(0x26);
	writeData(0x01);
	writeReg(0xE0);
	writeData(0x0F);
	writeData(0x2A);
	writeData(0x28);
	writeData(0x08);
	writeData(0x0E);
	writeData(0x08);
	writeData(0x54);
	writeData(0XA9);
	writeData(0x43);
	writeData(0x0A);
	writeData(0x0F);
	writeData(0x00);
	writeData(0x00);
	writeData(0x00);
	writeData(0x00);
	writeReg(0XE1);
	writeData(0x00);
	writeData(0x15);
	writeData(0x17);
	writeData(0x07);
	writeData(0x11);
	writeData(0x06);
	writeData(0x2B);
	writeData(0x56);
	writeData(0x3C);
	writeData(0x05);
	writeData(0x10);
	writeData(0x0F);
	writeData(0x3F);
	writeData(0x3F);
	writeData(0x0F);
	writeReg(0x2B);
	writeData(0x00);
	writeData(0x00);
	writeData(0x01);
	writeData(0x3f);
	writeReg(0x2A);
	writeData(0x00);
	writeData(0x00);
	writeData(0x00);
	writeData(0xef);
	writeReg(0x11);
	HAL_Delay(100);
	writeReg(0x29);

	fill(0, 0, 239, 39, BLUE);
	fill(0, 280, 239, 319, BLUE);
	drawPromotion();
}

void TftLcd::drawPromotion(){
	auto draw = [&](int x, Piece p){
		RgbColor back = x % 2 ? GREEN : GREY;
		array<uint32_t, 30> &pixels = *dict[p];
		for(int h = 0; h < 30; ++h){
			setCursor(x * 30, 29 - h);
			writeReg(0x2C);
			for(int w = 0; w < 30; ++w){
				writeData(pixels[h] & (1 << (29 - w)) ? US : back);
			}
		}
	};
	draw(0, W_QUEEN);
	draw(1, W_ROOK);
	draw(2, W_BISHOP);
	draw(3, W_KNIGHT);
}

void TftLcd::setCursor(uint16_t x, uint16_t y){
	writeReg(0x2A);
	writeData(x >> 8);
	writeData(x & 0XFF);
	writeReg(0x2B);
	writeData(y >> 8);
	writeData(y & 0XFF);
}

void TftLcd::writeReg(uint8_t reg){
	*(__IO uint16_t *)(0x60000000) = reg;
}

void TftLcd::writeData(uint16_t data){
	*(__IO uint16_t *)(0x60020000) = data;
}

map<Piece, array<uint32_t, 30> *> TftLcd::dict = {
	{NO_PIECE, &blank},
	{W_PAWN, &pawn},
	{B_PAWN, &pawn},
	{W_ROOK, &rook},
	{B_ROOK, &rook},
	{W_KNIGHT, &knight},
	{B_KNIGHT, &knight},
	{W_BISHOP, &bishop},
	{B_BISHOP, &bishop},
	{W_QUEEN, &queen},
	{B_QUEEN, &queen},
	{W_KING, &king},
	{B_KING, &king}
};

} /* namespace ehdu */
