#ifndef GCM_BASIC_RECT_METHOD_H
#define	GCM_BASIC_RECT_METHOD_H

#include "method/NumericalMethod.h"
#include "util/ElasticMatrix3D.h"
#include "mesh/cube/BasicCubicMesh.h"

namespace gcm {
	/*
	 * Numerical method
	 */
	class BasicRectMethod : public NumericalMethod {
	public:
		/*
		 * Constructor
		 */
		BasicRectMethod();
		/*
		 * Destructor
		 */
		~BasicRectMethod();
		/*
		 * Returns number of stages (part steps) this particular method requires
		 */
		int getNumberOfStages();
		/*
		 * Computes next state (after the next part step) for the given node
		 */
		void doNextPartStep(CalcNode* cur_node, CalcNode* new_node, float time_step, int stage, Mesh* genericMesh);
		/*
		 * Returns maximum lambda for the given node
		 */
		float getMaxLambda(CalcNode* node);
		
		string getType();
		
	protected:
		USE_LOGGER;
		
		int prepare_node(CalcNode* cur_node, ElasticMatrix3D* elastic_matrix3d,
												float time_step, int stage, BasicCubicMesh* mesh, 
												float* dksi, bool* inner, CalcNode* previous_nodes, 
												float* outer_normal, int* ppoint_num);
		int find_nodes_on_previous_time_layer(CalcNode* cur_node, int stage, BasicCubicMesh* mesh, 
												float dksi[], bool inner[], CalcNode previous_nodes[], 
												float outer_normal[], int ppoint_num[]);
		void interpolateNode(BasicCubicMesh* mesh, int curInd, int neighInd, int prevNodeInd, CalcNode* previous_nodes);
	};
}

#endif	/* GCM_BASIC_RECT_METHOD_H */
