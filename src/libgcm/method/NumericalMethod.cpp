/* 
 * File:   NumericalMethod.cpp
 * Author: anganar
 * 
 * Created on April 30, 2013, 8:18 PM
 */

#include "NumericalMethod.h"
#include "../node/CalcNode.h"

gcm::NumericalMethod::NumericalMethod() {
	spaceOrder = 1;
	timeOrder = 1;
}

gcm::NumericalMethod::~NumericalMethod() {
}

void gcm::NumericalMethod::setSpaceOrder(int order) {
	spaceOrder = order;
}

void gcm::NumericalMethod::setTimeOrder(int order) {
	timeOrder = order;
}

int gcm::NumericalMethod::getSpaceOrder() {
	return spaceOrder;
}

int gcm::NumericalMethod::getTimeOrder() {
	return timeOrder;
}