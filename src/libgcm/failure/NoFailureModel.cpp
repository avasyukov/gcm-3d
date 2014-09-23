#include "libgcm/failure/NoFailureModel.hpp"

using namespace gcm;

NoFailureModel::NoFailureModel() {
    type = "NoFailureModel";
    INIT_LOGGER( "gcm.NoFailureModel" );
}
