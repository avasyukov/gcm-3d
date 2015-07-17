#include "libgcm/calc/volume/SimpleVolumeCalculator.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::vector;

SimpleVolumeCalculator::SimpleVolumeCalculator() {
    INIT_LOGGER("gcm.SimpleVolumeCalculator");
};

void SimpleVolumeCalculator::doCalc(CalcNode& cur_node, CalcNode& new_node, RheologyMatrixPtr matrix,
                                                        vector<CalcNode>& previousNodes)
{
    assert_eq(previousNodes.size(), 9);

    LOG_TRACE("Start calc");

    // Here we will store (omega = Matrix_OMEGA * u)
    float omega[9];

    // Calculate omega value
    // TODO - should we use U and U^-1 from old or new node??? Most probably, the 1st from old and the 2nd from new.
    for(int i = 0; i < 9; i++)
    {
        // omega on new time layer is equal to omega on previous time layer along characteristic
        omega[i] = 0;
        for(int j = 0; j < 9; j++)
        {
            omega[i] += matrix->getU(i,j) * previousNodes[i].values[j];
        }
    }
    // Calculate new values
    for(int i = 0; i < 9; i++)
    {
        float new_val = 0;
        //new_node.values[i] = 0;
        for(int j = 0; j < 9; j++)
        {
            new_val += matrix->getU1(i,j) * omega[j];
        }
        new_node.values[i] = new_node.values[i] + (new_val - cur_node.values[i]);
    }
    return;
//WARNING Toxic hack ahead ----------------------------------------------------------------------------
    int nmax = 0, nmin = 0;
    gcm::real max = matrix->getL(0, 0), min = matrix->getL(0, 0);
    for(int i = 1; i < 9; i++)
    {
	if (matrix->getL(i, i) > max) {max = matrix->getL(i, i); nmax = i;};
        if (matrix->getL(i, i) < min) {min = matrix->getL(i, i); nmin = i;};
    }
    if (previousNodes[nmax].getMaterialId() != cur_node.getMaterialId() || previousNodes[nmin].getMaterialId() != cur_node.getMaterialId())
    {
	for(int i = 0; i < 9; i++)
	    new_node.values[i] = (previousNodes[nmax].values[i] + previousNodes[nmin].values[i])/2;
    }

//-----------------------------------------------------------------------------------------------------
    LOG_TRACE("Calc done");
};
