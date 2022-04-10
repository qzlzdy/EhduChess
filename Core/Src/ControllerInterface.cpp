/*
 * ControllerInterface.cpp
 *
 *  Created on: 2022年4月9日
 *      Author: qzlzdy
 */
#include "ControllerInterface.h"

namespace ehdu{

ControllerInterface::ControllerInterface(): ctrl(nullptr){

}

Controller *ControllerInterface::getController(){
	return ctrl;
}

void ControllerInterface::setController(Controller *c){
	ctrl = c;
}

}
