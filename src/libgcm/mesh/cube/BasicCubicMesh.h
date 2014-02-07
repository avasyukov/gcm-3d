#ifndef GCM_BASIC_CUBIC_MESH_H_
#define GCM_BASIC_CUBIC_MESH_H_

#include "mesh/Mesh.h"
#include "Math.h"
#include "Logging.h"
#include "Exception.h"

using namespace gcm;

namespace gcm
{
	class CalcNode;
	
	class BasicCubicMesh: public Mesh {
		
	// Hopefully we will never need these 'friends' back
/*	friend class VTKSnapshotWriter;
	friend class DataBus;
	friend class CollisionDetector;
	friend class BruteforceCollisionDetector;
*/		
	protected:
		void logMeshStats();
		void calcMinH();
		void preProcessGeometry();
		
		float meshH;
		
		USE_LOGGER;
		
	public:
		BasicCubicMesh();
		~BasicCubicMesh();
		
		float getRecommendedTimeStep();
		float getMinH();
		void doNextPartStep(float tau, int stage);
		void checkTopology(float tau);
		
		void findBorderNodeNormal(int border_node_index, float* x, float* y, float* z, bool debug);
		int findTargetPoint(CalcNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);


		int prepare_node(CalcNode* cur_node, ElasticMatrix3D* elastic_matrix3d,
														float time_step, int stage,
														float* dksi, bool* inner, CalcNode* previous_nodes,
														float* outer_normal, int* ppoint_num);
		int find_nodes_on_previous_time_layer(CalcNode* cur_node, int stage,
														float dksi[], bool inner[], CalcNode previous_nodes[],
														float outer_normal[], int ppoint_num[]);


		void interpolateNode(int curInd, int neighInd, int prevNodeInd, CalcNode* previous_nodes);
	};
}
#endif
