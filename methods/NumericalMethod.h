#ifndef _GCM_NUMERICAL_METHOD_H
#define _GCM_NUMERICAL_METHOD_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "../system/GCMException.h"
#include "../system//LoggerUser.h"

class NumericalMethod: protected LoggerUser
{
public:
	NumericalMethod();
	~NumericalMethod();
	virtual int get_number_of_stages() = 0;
	string* get_num_method_type();
protected:
	string num_method_type;
};

#endif
