#ifndef _GCM_NUMERICAL_METHOD_H
#define _GCM_NUMERICAL_METHOD_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "../system/Logger.h"

class NumericalMethod
{
public:
	NumericalMethod();
	~NumericalMethod();
	void attach(Logger* new_logger);
	virtual int get_number_of_stages() = 0;
	string* get_num_method_type();
protected:
	Logger* logger;
	string num_method_type;
};

#endif
