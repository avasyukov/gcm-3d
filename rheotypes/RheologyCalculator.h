#ifndef _GCM_RHEOLOGY_H
#define _GCM_RHEOLOGY_H  1

#include <string>
#include <vector>

#include "../system/logger.h"

using std::string;
using std::vector;

class RheologyCalculator
{
public:
	RheologyCalculator();
	~RheologyCalculator();
	int do_calc(Node* cur_node, Node* new_node);
	void attach(Logger* new_logger);
protected:
	Logger* logger;
};

#include "RheologyCalculator.inl"

#endif
