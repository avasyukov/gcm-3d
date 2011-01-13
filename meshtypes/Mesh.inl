Mesh::Mesh()
{
	logger = NULL;
	rheology = NULL;
//	method = NULL;
	data_bus = NULL;
	stresser = NULL;
	mesh_type.assign("Generic mesh");
	current_time = 0;
};

Mesh::~Mesh() { };

void Mesh::attach(Logger* new_logger)
{
	logger = new_logger;
	if(logger != NULL)
		logger->write(string("Attached logger. Type: ") + *(logger->get_logger_type()));

	if(rheology != NULL)
		rheology->attach(logger);
//	if(method != NULL)
//		method->attach(logger);
	if(data_bus != NULL)
		data_bus->attach(logger);
	if(stresser != NULL)
		stresser->attach(logger);
};

/*void Mesh::attach(NumericalMethod* new_numerical_method)
{
	method = new_numerical_method;
	if(method != NULL)
	{
		method->attach(logger);
		if(logger != NULL)
		{
			logger->write(string("Attached numerical method. Type: ") + *(method->get_num_method_type()));
		}
	}
};*/

void Mesh::attach(DataBus* new_data_bus)
{
	data_bus = new_data_bus;
	if(data_bus != NULL)
	{
		data_bus->attach(logger);
		if(logger != NULL)
		{
			logger->write(string("Attached numerical method. Type: ") + *(data_bus->get_data_bus_type()));
		}
	}
};

void Mesh::attach(Stresser* new_stresser)
{
	stresser = new_stresser;
	if(stresser != NULL)
	{
		stresser->attach(logger);
		if(logger != NULL)
		{
			logger->write(string("Attached numerical method. Type: ") + *(stresser->get_stresser_type()));
		}
	}
};

void Mesh::attach(RheologyCalculator* new_rheology)
{
	rheology = new_rheology;
	if(rheology != NULL)
	{
		rheology->attach(logger);
		if(logger != NULL)
		{
			logger->write(string("Attached rheology calculator. Type: ") + *(rheology->get_rheology_type()));
		}
	}
};

string* Mesh::get_mesh_type()
{
	return &mesh_type;
};

float Mesh::get_current_time()
{
	return current_time;
};
