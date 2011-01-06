#ifndef _GCM_VOID_RHEOLOGY_H
#define _GCM_VOID_RHEOLOGY_H  1

#include "RheologyCalculator.h"

class VoidRheologyCalculator : public RheologyCalculator
{
public:
	VoidRheologyCalculator();
	~VoidRheologyCalculator();
	int do_calc(Node* cur_node, Node* new_node);
};

#include "VoidRheologyCalculator.inl"

#endif
