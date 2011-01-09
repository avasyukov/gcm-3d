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
	virtual int get_number_of_stages() = 0;
	virtual float get_max_lambda(ElasticNode* node) = 0;
	string* get_num_method_type();
protected:
	Logger* logger;
	string num_method_type;
};

#include "NumericalMethod.inl"

#endif
