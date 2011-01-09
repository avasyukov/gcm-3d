DataBus::DataBus()
{
	data_bus_type.assign("Stupid echo data bus");
};

DataBus::~DataBus() { };

string* DataBus::get_data_bus_type()
{
	return &data_bus_type;
};

void DataBus::attach(Logger* new_logger)
{
	logger = new_logger;
};

float DataBus::get_max_possible_tau(float local_time_step)
{
	return local_time_step;
};

int DataBus::sync_nodes()
{
	return 0;
};
