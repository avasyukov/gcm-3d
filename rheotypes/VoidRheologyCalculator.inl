VoidRheologyCalculator::VoidRheologyCalculator()
{
	rheology_type.assign("Void rheology");
};

VoidRheologyCalculator::~VoidRheologyCalculator() { };

int VoidRheologyCalculator::do_calc(ElasticNode* cur_node, ElasticNode* new_node)
{
	return 0;
};
