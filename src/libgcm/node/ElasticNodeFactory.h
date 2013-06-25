#ifndef GCM_ELASTIC_NODE_FACTORY_H_
#define GCM_ELASTIC_NODE_FACTORY_H_

#include <string>

#include "ElasticNode.h"
#include "NodeFactory.h"
#include "../Logging.h"

using namespace std;

namespace gcm {
	class ElasticNodeFactory: public TemplatedNodeFactory<ElasticNode> {
	protected:
		/*
		 * Creates MPI type.
		 */
		MPI::Datatype createDataType();
		USE_LOGGER;
	public:
		ElasticNodeFactory();
		/*
		 * Returns type of the factory.
		 */
		string getType();
	};
}
#endif