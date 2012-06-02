#include "RheologyCalculator.h"

RheologyCalculator::RheologyCalculator()
{
	rheology_type.assign("Generic rheology");
};

RheologyCalculator::~RheologyCalculator() { };

string* RheologyCalculator::get_rheology_type()
{
	return &rheology_type;
}
