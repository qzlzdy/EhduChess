/*
 * LoopInterface.h
 *
 *  Created on: 2022年4月9日
 *      Author: qzlzdy
 */

#ifndef INC_LOOPINTERFACE_H_
#define INC_LOOPINTERFACE_H_

namespace ehdu{

class LoopInterface{
public:
	LoopInterface() = default;
	LoopInterface(const LoopInterface &) = default;
	LoopInterface &operator=(const LoopInterface &) = default;
	LoopInterface(LoopInterface &&) = default;
	LoopInterface &operator=(LoopInterface &&) = default;
	virtual ~LoopInterface() = default;
	virtual void process() = 0;
};

}

#endif /* INC_LOOPINTERFACE_H_ */
