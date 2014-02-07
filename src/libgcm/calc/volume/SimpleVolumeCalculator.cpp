#include "calc/volume/SimpleVolumeCalculator.h"

#include "node/CalcNode.h"

SimpleVolumeCalculator::SimpleVolumeCalculator() {
	INIT_LOGGER("gcm.SimpleVolumeCalculator");
};

void SimpleVolumeCalculator::do_calc(CalcNode* new_node, ElasticMatrix3D* matrix, float* values[])
{
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
			omega[i] += matrix->U(i,j) * values[i][j];
		}
	}
	// Calculate new values
	for(int i = 0; i < 9; i++)
	{
		new_node->values[i] = 0;
		for(int j = 0; j < 9; j++)
		{
			new_node->values[i] += matrix->U1(i,j) * omega[j];
		}
	}
	LOG_TRACE("Calc done");
};
