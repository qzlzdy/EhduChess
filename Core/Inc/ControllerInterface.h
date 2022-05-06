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
	virtual ~ControllerInterface() = default;
	virtual void acceptMove() = 0;
	virtual void process() = 0;
	virtual void select(int x, int y) = 0;
	virtual void touch(int x) = 0;
	virtual void touch(int x, int y) = 0;
	virtual void unselect() = 0;
	virtual void setBestmove(const std::string &m) = 0;
protected:
	enum CtrlState{
		CTRL_WAIT,
		CTRL_SELECTED,
		CTRL_MOVED,
		CTRL_PROMOTION,
		CTRL_COMPUTING,
		CTRL_FINISHED
	};
	CtrlState state;
	std::string currmove;
	std::string bestmove;
	int selx, sely;
};

}

#endif /* INC_CONTROLLERINTERFACE_H_ */
