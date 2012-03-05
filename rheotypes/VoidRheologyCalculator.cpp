#include "VoidRheologyCalculator.h"

VoidRheologyCalculator::VoidRheologyCalculator()
{
	rheology_type.assign("Void rheology");
};

VoidRheologyCalculator::~VoidRheologyCalculator() { };

int VoidRheologyCalculator::do_calc(ElasticNode* cur_node, ElasticNode* new_node)
{
	if((cur_node == NULL) || (new_node == NULL))
		return -1;
	return 0;
};
