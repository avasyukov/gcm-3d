#include "libgcm/failure/FailureModel.hpp"

gcm::FailureModel::FailureModel() {
    type = "FailureModel";
    INIT_LOGGER( "gcm.FailureModel" );
}

gcm::FailureModel::~FailureModel() {
    /*for(auto criterion : criteria) {
        delete criterion;
    }
    for(auto corrector : correctors) {
        delete corrector;
    }*/
}

string gcm::FailureModel::getType() {
    return type;
}

void gcm::FailureModel::checkFailure(ICalcNode& node, const float tau) {
    for(auto criterion : criteria) {
        criterion->checkFailure(node, tau);
    }
}

void gcm::FailureModel::applyCorrection(ICalcNode& node, const float tau) {
    for(auto corrector : correctors) {
        corrector->applyCorrection(node, tau);
    }
}