/*
 * TouchScreen.h
 *
 *  Created on: 2022年4月6日
 *      Author: qzlzdy
 */

#ifndef INC_TOUCHSCREEN_H_
#define INC_TOUCHSCREEN_H_

#include <BackwardInterface.h>

namespace ehdu{

class TouchScreen: public ControllerInterface{
public:
	TouchScreen(const TouchScreen &) = delete;
	TouchScreen &operator=(const TouchScreen &) = delete;
	TouchScreen(TouchScreen &&) = default;
	TouchScreen &operator=(TouchScreen &&) = default;
	virtual ~TouchScreen() = default;
protected:
	TouchScreen() = default;
};

}

#endif /* INC_TOUCHSCREEN_H_ */
