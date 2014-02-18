#include "rheology/StdRheologyCalculator.h"

#include "node/CalcNode.h"

gcm::StdRheologyCalculator::StdRheologyCalculator() {
	INIT_LOGGER("gcm.StdRheologyCalculator");
}

void gcm::StdRheologyCalculator::doCalc(CalcNode& src, CalcNode& dst)
{
	float K = src.getLambda() + 2 * src.getMu() / 3;
	dst.setRho( src.getRho0() * expf( src.getPressure() / K ) );
}
