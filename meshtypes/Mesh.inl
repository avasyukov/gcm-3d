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
	if(logger != NULL)
		logger->write(string("ERROR: Called unimplemented next step on mesh!"));
	return -1;
};

int Mesh::get_index_of_element_owner(Node* node)
{
	if(logger != NULL)
		logger->write(string("ERROR: Called unimplemented element search on mesh!"));
	return -1;
};

string* Mesh::get_mesh_type()
{
	return &mesh_type;
};
