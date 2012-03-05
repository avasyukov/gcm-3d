#ifndef _GCM_VOID_RHEOLOGY_H
#define _GCM_VOID_RHEOLOGY_H  1

#include "RheologyCalculator.h"

class VoidRheologyCalculator : public RheologyCalculator
{
public:
	VoidRheologyCalculator();
	~VoidRheologyCalculator();
	int do_calc(ElasticNode* cur_node, ElasticNode* new_node);
};

#endif
