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
