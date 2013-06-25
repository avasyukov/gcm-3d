#include "ElasticNodeFactory.h"

gcm::ElasticNodeFactory::ElasticNodeFactory() {
	INIT_LOGGER("gcm.ElasticNodeFactory");
}

string gcm::ElasticNodeFactory::getType() {
	return "elastic";
}

MPI::Datatype gcm::ElasticNodeFactory::createDataType() {
	ElasticNode node;

	MPI::Datatype types[] = {
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT
	};

	int lengths[] = {
		3,
		9,
		3
	};

	MPI::Aint displacements[] = {
		MPI::Get_address(node.coords),
		MPI::Get_address(node.values),
		MPI::Get_address(node.elasticRheologyProperties)
	};

	for (int i = 2; i >=0; i--)
		displacements[i] -= displacements[0];

    return MPI::Datatype::Create_struct(
    	3,
    	lengths,
    	displacements,
    	types     
    );
}