#include "NumericalMethod.h"

NumericalMethod::NumericalMethod()
{
	num_method_type.assign("Generic method");
};

NumericalMethod::~NumericalMethod() { };

string* NumericalMethod::get_num_method_type()
{
	return &num_method_type;
};
