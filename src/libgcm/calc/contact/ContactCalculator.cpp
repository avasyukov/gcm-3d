/*
 * File:   ContactCalculator.cpp
 * Author: anganar
 *
 * Created on April 30, 2013, 3:27 PM
 */
#include "libgcm/calc/contact/ContactCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::string;

ContactCalculator::~ContactCalculator() {
}

string ContactCalculator::getType()
{
	return type;
}
