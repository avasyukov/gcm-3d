#ifndef GCM_NODE_FACTORY_H_
#define GCM_NODE_FACTORY_H_

#include <string>
#include <mpi.h>

#include "Node.h"
#include "../Logging.h"

using namespace std;

namespace gcm {
	/*
	 * Produces nodes of specific type.
	 */
	class NodeFactory {
	private:
		/*
		 * Status flag.
		 */
		bool initialized;
		/*
		 * MPI type to hold node information.
		 */
		MPI::Datatype MPIType;
	protected:
		/*
		 * Creates MPI type.
		 */
		virtual MPI::Datatype createDataType() = 0;		 
		
		USE_LOGGER;
	public:
		/*
		 * Constructor.
		 */
		NodeFactory();
		/*
		 * Returns pointer to array of nodes with length of number.
		 */
		virtual Node* makeNodes(int number) = 0;
		/*
		 * Frees memory allocated for nodes.
		 */
		virtual void destroyNodes(Node* nodes) = 0;
		/*
		 * Returns type of the factory.
		 */
		virtual string getType() = 0;
		/*
		 * Returns MPI data type for node.
		 */
		MPI::Datatype getMPIDataType();
	};

	template<typename NodeType>
	class TemplatedNodeFactory: public NodeFactory {
	public:
		/*
		 * Returns pointer to array of nodes with length of number.
		 */
		NodeType* makeNodes(int number) {
			LOG_DEBUG("Creating new nodes. Type: " << getType() << " Number: " << number);
			return new NodeType[number];
		}
		/*
		 * Frees memory allocated for nodes.
		 */
		void destroyNodes(NodeType* nodes) {
			LOG_DEBUG("Deleting nodes. Type: " << getType());
			if( nodes != NULL )
				delete[] nodes;
		}
		void destroyNodes(Node* nodes) {
			LOG_DEBUG("Deleting nodes. Generic call.");
			if( nodes != NULL )
				delete[] (NodeType*)nodes;
		}
	};
}
#endif