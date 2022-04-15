/*
 * ControllerInterface.h
 *
 *  Created on: 2022年4月9日
 *      Author: qzlzdy
 */

#ifndef INC_BACKWARDINTERFACE_H_
#define INC_BACKWARDINTERFACE_H_

#include "ControllerInterface.h"

namespace ehdu{

class BackwardInterface{
public:
	BackwardInterface();
	BackwardInterface(const BackwardInterface &) = default;
	BackwardInterface &operator=(const BackwardInterface &) = default;
	BackwardInterface(BackwardInterface &&) = default;
	BackwardInterface &operator=(BackwardInterface &&) = default;
	virtual ~BackwardInterface() = default;
	virtual ControllerInterface *getController();
	virtual void setController(ControllerInterface *c);
private:
	ControllerInterface *ctrl;
};

}

#endif /* INC_BACKWARDINTERFACE_H_ */
