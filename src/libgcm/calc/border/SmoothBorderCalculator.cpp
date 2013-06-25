#include "SmoothBorderCalculator.h"

SmoothBorderCalculator::SmoothBorderCalculator()
{
	INIT_LOGGER( "gcm.SmoothBorderCalculator" );
};

SmoothBorderCalculator::~SmoothBorderCalculator()
{
};

void SmoothBorderCalculator::do_calc(ElasticNode* new_node, ElasticMatrix3D* matrix, float* values[], bool inner[], float outer_normal[], float scale)
{
	LOG_TRACE("Starting calc with SmoothBorderCalculator");
	
	int inner_node_num = -1;
	int outer_count = 0;
	for(int i = 0; i < 9; i++)
		if( inner[i] )
			inner_node_num = i;
		else
			outer_count++;
	
	assert( outer_count != 3 );
	
	for(int j = 0; j < 9; j++)
		new_node->values[j] = values[inner_node_num][j];

	LOG_TRACE("Calc done");
};
