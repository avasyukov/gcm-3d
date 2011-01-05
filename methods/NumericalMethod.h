#ifndef _GCM_NUMERICAL_METHOD_H
#define _GCM_NUMERICAL_METHOD_H  1

#include <string>
#include <vector>

#include "../system/logger.h"

using std::string;
using std::vector;

class NumericalMethod
{
public:
	NumericalMethod();
	~NumericalMethod();
	void attach(Logger* new_logger);
	int do_next_part_step(Node* cur_node, Node* new_node, float time_step, int stage);
	int get_number_of_stages();
protected:
	Logger* logger;
};

#include "NumericalMethod.inl"

#endif
