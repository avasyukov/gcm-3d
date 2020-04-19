#include "libgcm/failure/FailureModel.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/linal/Matrix33.hpp"
#include "libgcm/linal/Vector3.hpp"

using namespace gcm;
using namespace gcm::linal;
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
	CalcNode node1 = prepare_node(node);

    for(auto criterion : criteria) {
        criterion->checkFailure(node1, tau);
    }

    node.setDestroyed(node1.isDestroyed());
    node.setDamageMeasure(node1.getDamageMeasure());
    node.createCrack(node1.getCrackDirection()[0], node1.getCrackDirection()[1], node1.getCrackDirection()[2]);
}

void FailureModel::applyCorrection(ICalcNode& node, const float tau) {
    for(auto corrector : correctors) {
        corrector->applyCorrection(node, tau);
    }
}

CalcNode FailureModel::prepare_node(ICalcNode& node)
{
	CalcNode result = static_cast<CalcNode&>(node);

	Matrix33 mat({
		node.sxx,	node.sxy,	node.sxz,
		node.sxy,	node.syy,	node.syz,
		node.sxz,	node.syz,	node.szz
	});

	const vector3r& angles = node.getMaterial()->getAngles();

	Matrix33 rot = rotate(mat, -angles[0], -angles[1], -angles[2]);

	result.sxx = rot(0, 0);	result.sxy = rot(0, 1);	result.sxz = rot(0, 2);
	result.syy = rot(1, 1);	result.syz = rot(1, 2);
	result.szz = rot(2, 2);

	return result;
}
