#include "libgcm/interpolator/Interpolator.hpp"

using std::string;
using namespace gcm;

Interpolator::Interpolator() {
    type = "Interpolator";
    INIT_LOGGER( "gcm.Interpolator" );
}

Interpolator::~Interpolator() {
}

string Interpolator::getType() {
    return type;
}
