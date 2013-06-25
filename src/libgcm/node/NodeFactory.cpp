#include "NodeFactory.h"

gcm::NodeFactory::NodeFactory() {
	initialized = false;
	INIT_LOGGER("gcm.NodeFactory");
}

MPI::Datatype gcm::NodeFactory::getMPIDataType() {
	if (!initialized)
		MPIType = createDataType();
	return MPIType;
}