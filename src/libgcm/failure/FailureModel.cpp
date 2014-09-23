#include "libgcm/failure/FailureModel.hpp"

using namespace gcm;
using std::string;

FailureModel::FailureModel() {
    type = "FailureModel";
    INIT_LOGGER( "gcm.FailureModel" );
}

FailureModel::~FailureModel() {
    /*for(auto criterion : criteria) {
        delete criterion;
    }
    for(auto corrector : correctors) {
        delete corrector;
    }*/
}

string FailureModel::getType() {
    return type;
}

void FailureModel::checkFailure(ICalcNode& node, const float tau) {
    for(auto criterion : criteria) {
        criterion->checkFailure(node, tau);
    }
}

void FailureModel::applyCorrection(ICalcNode& node, const float tau) {
    for(auto corrector : correctors) {
        corrector->applyCorrection(node, tau);
    }
}
