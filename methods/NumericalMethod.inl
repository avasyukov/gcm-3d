NumericalMethod::NumericalMethod()
{
	logger = NULL;
	num_method_type.assign("Generic method");
};

NumericalMethod::~NumericalMethod() { };

void NumericalMethod::attach(Logger* new_logger)
{
	logger = new_logger;
};

string* NumericalMethod::get_num_method_type()
{
	return &num_method_type;
};

int NumericalMethod::do_next_part_step(Node* cur_node, Node* new_node, float time_step, int stage)
{
	if(logger != NULL)
		logger->write(string("ERROR: Called unimplemented method for next part step!"));
	return -1;
};

int NumericalMethod::get_number_of_stages()
{
	if(logger != NULL)
		logger->write(string("ERROR: Called unimplemented method for number of stages!"));
	return -1;
};

float NumericalMethod::get_max_lambda(Node* node)
{
	if(logger != NULL)
		logger->write(string("ERROR: Called unimplemented method for max lambda!"));
	return -1;
};
