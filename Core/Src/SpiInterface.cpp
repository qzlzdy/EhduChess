/*
 * SpiInterface.cpp
 *
 *  Created on: 2022年4月10日
 *      Author: qzlzdy
 */
#include "SpiInterface.h"

namespace ehdu{

SpiInterface::SpiInterface(): hspi(nullptr){

}

SPI_HandleTypeDef *SpiInterface::getSpi(){
	return hspi;
}

void SpiInterface::setSpi(SPI_HandleTypeDef *spi){
	hspi = spi;
}

}
