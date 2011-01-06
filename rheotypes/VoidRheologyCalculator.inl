VoidRheologyCalculator::VoidRheologyCalculator()
{
	rheology_type.assign("Void rheology");
};

VoidRheologyCalculator::~VoidRheologyCalculator() { };

int VoidRheologyCalculator::do_calc(Node* cur_node, Node* new_node)
{
	return 0;
};
