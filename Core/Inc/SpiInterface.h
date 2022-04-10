/*
 * SpiInterface.h
 *
 *  Created on: 2022年4月10日
 *      Author: qzlzdy
 */

#ifndef INC_SPIINTERFACE_H_
#define INC_SPIINTERFACE_H_

#include "main.h"

namespace ehdu{

class SpiInterface{
public:
	SpiInterface();
	SpiInterface(const SpiInterface &) = default;
	SpiInterface &operator=(const SpiInterface &) = default;
	SpiInterface(SpiInterface &&) = default;
	SpiInterface &operator=(SpiInterface &&) = default;
	virtual ~SpiInterface() = default;
	SPI_HandleTypeDef *getSpi();
	void setSpi(SPI_HandleTypeDef *spi);
private:
	SPI_HandleTypeDef *hspi;
};

}

#endif /* INC_SPIINTERFACE_H_ */
