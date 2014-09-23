/*
 * File:   NumericalMethod.cpp
 * Author: anganar
 *
 * Created on April 30, 2013, 8:18 PM
 */
#include "libgcm/method/NumericalMethod.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;

NumericalMethod::NumericalMethod() {
    spaceOrder = 1;
    timeOrder = 1;
}

NumericalMethod::~NumericalMethod() {
}

void NumericalMethod::setSpaceOrder(int order) {
    spaceOrder = order;
}

void NumericalMethod::setTimeOrder(int order) {
    timeOrder = order;
}

int NumericalMethod::getSpaceOrder() {
    return spaceOrder;
}

int NumericalMethod::getTimeOrder() {
    return timeOrder;
}
