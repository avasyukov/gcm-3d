#include "libgcm/failure/NoFailureModel.hpp"

gcm::NoFailureModel::NoFailureModel() {
    type = "NoFailureModel";
    INIT_LOGGER( "gcm.NoFailureModel" );
}