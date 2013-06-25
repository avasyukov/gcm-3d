#include "TetrInterpolator.h"

gcm::TetrInterpolator::TetrInterpolator() {
	type = "Interpolator";
	INIT_LOGGER( "gcm.Interpolator" );
}

gcm::TetrInterpolator::~TetrInterpolator() {
}

string gcm::TetrInterpolator::getType() {
	return type;
}