#include "interpolator/Interpolator.h"

gcm::Interpolator::Interpolator() {
	type = "Interpolator";
	INIT_LOGGER( "gcm.Interpolator" );
}

gcm::Interpolator::~Interpolator() {
}

string gcm::Interpolator::getType() {
	return type;
}
