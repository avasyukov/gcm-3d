#include "libgcm/interpolator/TetrInterpolator.hpp"

using namespace gcm;
using std::string;

TetrInterpolator::TetrInterpolator() {
    type = "Interpolator";
    INIT_LOGGER( "gcm.Interpolator" );
}

TetrInterpolator::~TetrInterpolator() {
}

string TetrInterpolator::getType() {
    return type;
}
