RheologyCalculator::RheologyCalculator()
{
	logger = NULL;
	rheology_type.assign("Generic rheology");
};

RheologyCalculator::~RheologyCalculator() { };

void RheologyCalculator::attach(Logger* new_logger) { logger = new_logger; };

string* RheologyCalculator::get_rheology_type()
{
	return &rheology_type;
}
