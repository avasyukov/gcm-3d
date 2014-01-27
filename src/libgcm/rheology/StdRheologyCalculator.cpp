#include "StdRheologyCalculator.h"
#include "../node/CalcNode.h"

gcm::StdRheologyCalculator::StdRheologyCalculator() {
	INIT_LOGGER("gcm.StdRheologyCalculator");
}

gcm::StdRheologyCalculator::~StdRheologyCalculator() {
	
}

void gcm::StdRheologyCalculator::doCalc(CalcNode* src, CalcNode* dst)
{
	//Calculating rho in this node with state equation from page 13
	float rho0 = src->getRho0();
	float lambda = src->getLambda();
	float mu = src->getMu();
	float K = lambda + 2*mu/3;
	float pressure = src->getPressure();
	float rho_new = rho0*exp(pressure/K);
	dst->setRho(rho_new);
}
