/*
 * ControllerInterface.h
 *
 *  Created on: 2022年4月9日
 *      Author: qzlzdy
 */

#ifndef INC_CONTROLLERINTERFACE_H_
#define INC_CONTROLLERINTERFACE_H_

#include "Controller.h"

namespace ehdu{

class ControllerInterface{
public:
	ControllerInterface();
	ControllerInterface(const ControllerInterface &) = default;
	ControllerInterface &operator=(const ControllerInterface &) = default;
	ControllerInterface(ControllerInterface &&) = default;
	ControllerInterface &operator=(ControllerInterface &&) = default;
	virtual ~ControllerInterface() = default;
	virtual Controller *getController();
	virtual void setController(Controller *c);
private:
	Controller *ctrl;
};

}

#endif /* INC_CONTROLLERINTERFACE_H_ */
