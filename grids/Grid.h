#ifndef _GCM_GRID_H
#define _GCM_GRID_H  1

#include <string>
#include <vector>

#include "../system/logger.h"
#include "../rheotypes/RheologyCalculator.h"
#include "../methods/NumericalMethod.h"

using std::string;
using std::vector;

class Grid
{
public:
	Grid();
	~Grid();
	void attach(Logger* new_logger);
	void attach(NumericalMethod* new_numerical_method);
	void attach(RheologyCalculator* new_rheology);
protected:
	Logger* logger;
	RheologyCalculator* rheology;
	NumericalMethod* method;
};

#include "Grid.inl"

#endif
