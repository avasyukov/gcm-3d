#ifndef _GCM_RHEOLOGY_H
#define _GCM_RHEOLOGY_H  1

#include <string>
#include <vector>

#include "../datatypes/ElasticNode.h"
#include "../system/Logger.h"

using std::string;
using std::vector;

class RheologyCalculator
{
public:
	RheologyCalculator();
	~RheologyCalculator();
	virtual int do_calc(ElasticNode* cur_node, ElasticNode* new_node) = 0;
	void attach(Logger* new_logger);
	string* get_rheology_type();
protected:
	Logger* logger;
	string rheology_type;
};

#include "RheologyCalculator.inl"

#endif
