#ifndef _GCM_NUMERICAL_METHOD_H
#define _GCM_NUMERICAL_METHOD_H  1

#include <string>
#include <vector>

#include "../system/Logger.h"

using std::string;
using std::vector;

class NumericalMethod
{
public:
	NumericalMethod();
	~NumericalMethod();
	void attach(Logger* new_logger);
	virtual int do_next_part_step(Node* cur_node, Node* new_node, float time_step, int stage);
	virtual int get_number_of_stages();
	virtual float get_max_lambda(Node* node);
	string* get_num_method_type();
protected:
	Logger* logger;
	string num_method_type;
};

#include "NumericalMethod.inl"

#endif
