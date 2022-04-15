/*
 * ControllerInterface.h
 *
 *  Created on: 2022年4月13日
 *      Author: qzlzdy
 */

#ifndef INC_CONTROLLERINTERFACE_H_
#define INC_CONTROLLERINTERFACE_H_

#include <string>

namespace ehdu{

class ControllerInterface{
public:
	ControllerInterface() = default;
	ControllerInterface(const ControllerInterface &) = default;
	ControllerInterface &operator=(const ControllerInterface &) = default;
	ControllerInterface(ControllerInterface &&) = default;
	ControllerInterface &operator=(ControllerInterface &&) = default;
	virtual ~ControllerInterface();
	virtual void touch(int x, int y) = 0;
	virtual void setBestmove(const std::string &m) = 0;
};

}

#endif /* INC_CONTROLLERINTERFACE_H_ */
