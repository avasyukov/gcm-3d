RheologyCalculator::RheologyCalculator() { };
RheologyCalculator::~RheologyCalculator() { };

void RheologyCalculator::attach(Logger* new_logger) { logger = new_logger; };

int RheologyCalculator::do_calc(Node* cur_node, Node* new_node)
{
	if(logger != NULL)
		logger->write(string("ERROR: Called unimplemented rheology calculation!"));
	return -1;
};
