Mesh::Mesh()
{
	logger = NULL;
	rheology = NULL;
	method = NULL;
	mesh_type.assign("Generic mesh");
};

Mesh::~Mesh() { };

void Mesh::attach(Logger* new_logger)
{
	logger = new_logger;
	logger->write(string("Attached logger. Type: ") + *(logger->get_logger_type()));
};

void Mesh::attach(NumericalMethod* new_numerical_method)
{
	method = new_numerical_method;
	if(logger != NULL)
	{
		logger->write(string("Attached numerical method. Type: ") + *(method->get_num_method_type()));
	}
};

void Mesh::attach(RheologyCalculator* new_rheology)
{
	rheology = new_rheology;
	if(logger != NULL)
	{
		logger->write(string("Attached rheology calculator. Type: ") + *(rheology->get_rheology_type()));
	}
};

int Mesh::step()
{
};
