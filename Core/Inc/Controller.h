/*
 * Controller.h
 *
 *  Created on: 2022年3月12日
 *      Author: qzlzdy
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include "ControllerInterface.h"
#include "Stockfish.h"

#include <array>
#include <cstdint>
#include <string>

namespace ehdu {

class Controller: public ControllerInterface, public Position{
public:
	Controller();
	Controller(const Controller &) = default;
	Controller &operator=(const Controller &) = default;
	Controller(Controller &&) = default;
	Controller &operator=(Controller &&) = default;
	~Controller() = default;
	void acceptMove() override;
	void process() override;
	void select(int x, int y) override;
	void touch(int x) override;
	void touch(int x, int y) override;
	void unselect() override;
	void setBestmove(const std::string &m) override;
};

} /* namespace ehdu */

#endif /* INC_CONTROLLER_H_ */
