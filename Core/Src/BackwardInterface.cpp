/*
 * ControllerInterface.cpp
 *
 *  Created on: 2022年4月9日
 *      Author: qzlzdy
 */
#include <BackwardInterface.h>

namespace ehdu{

BackwardInterface::BackwardInterface(): ctrl(nullptr){

}

ControllerInterface *BackwardInterface::getController(){
	return ctrl;
}

void BackwardInterface::setController(ControllerInterface *c){
	ctrl = c;
}

}
