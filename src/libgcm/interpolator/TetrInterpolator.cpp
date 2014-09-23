#include "libgcm/interpolator/TetrInterpolator.hpp"

gcm::TetrInterpolator::TetrInterpolator() {
    type = "Interpolator";
    INIT_LOGGER( "gcm.Interpolator" );
}

gcm::TetrInterpolator::~TetrInterpolator() {
}

string gcm::TetrInterpolator::getType() {
    return type;
}
