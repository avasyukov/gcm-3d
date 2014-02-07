#ifndef GCM_TETR_MESH_FIRST_ORDER_H_
#define GCM_TETR_MESH_FIRST_ORDER_H_

#include <unordered_map>
#include <algorithm>

#include <gsl/gsl_linalg.h>

#include "mesh/tetr/TetrMesh.h"
#include "elem/TetrFirstOrder.h"
#include "elem/TriangleFirstOrder.h"
#include "util/AABB.h"
#include "method/NumericalMethod.h"
#include "Logging.h"
#include "Exception.h"
#include "Math.h"

using namespace gcm;
using namespace std;

typedef unordered_map<int, int>::const_iterator MapIter;

namespace gcm
{
	class CalcNode;
	/*
	 * Tetrahedral 1st order mesh.
	 */
	class TetrMeshFirstOrder: public TetrMesh {
		
	friend class VTKSnapshotWriter;
	friend class DataBus;
	friend class CollisionDetector;
	friend class BruteforceCollisionDetector;
		
	protected:
		unordered_map<int, int> tetrsMap;
		
		// Cache for characteristics hits
		bool charactCacheAvailable();
		bool checkCharactCache(CalcNode* node, float dx, float dy, float dz, int& tetrNum);
		void updateCharactCache(CalcNode* node, float dx, float dy, float dz, int tetrNum);
		int getCharactCacheIndex(CalcNode* node, float dx, float dy, float dz);
		unordered_map<int, int> charactCache[12];
		unsigned long long cacheHits;
		unsigned long long cacheMisses;
		
		/*
		 * List of mesh tetrahedrons.
		 */
		vector<TetrFirstOrder> tetrs1;
		vector<TriangleFirstOrder> border1;

		int tetrsNumber;
		int tetrsStorageSize;
		int faceNumber;
		int faceStorageSize;
		
		void build_surface_reverse_lookups();
		void build_volume_reverse_lookups();
		void build_border();
		void check_numbering();
		void check_outer_normals();
		void check_unused_nodes();
		void verifyTetrahedraVertices ();
		
		bool isTriangleBorder(int v[4], bool* needSwap, bool debug);
		bool isTriangleBorder(int v[4], bool* needSwap);
		bool isTriangleBorder(int v[4], bool debug);
		bool isTriangleBorder(int v[4]);
		/*
		 * Adds triangle to border and simultaneously checks its orientation
		 */
		TriangleFirstOrder createBorderTriangle(int v[4], int number);
		void find_border_elem_normal(int border_element_index, float* x, float* y, float* z);
		
		float get_solid_angle(int node_index, int tetr_index);
		float tetr_h(int i);
		
		void logMeshStats();
	
		/*TetrFirstOrder*/ int find_border_cross(CalcNode* node, float dx, float dy, float dz, bool debug, float* cross);
		/*TetrFirstOrder*/ int find_border_cross(CalcNode* node, float dx, float dy, float dz, bool debug, CalcNode* cross);
		
		void calcMinH();
		void calcAvgH();
		void calcMaxH();
		// It MUST take into account mesh topology.
		// So, if the mesh will be second order, h = h / 2, etc
		float mesh_min_h;
		float mesh_avg_h;
		float mesh_max_h;

		USE_LOGGER;
		
		/*TetrFirstOrder*/ int expandingScanForPoint (CalcNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
		/*TetrFirstOrder*/ int expandingScanForOwnerTetr(CalcNode* node, float dx, float dy, float dz, bool debug);
		/*TetrFirstOrder*/ int fastScanForOwnerTetr(CalcNode* node, float dx, float dy, float dz, bool debug);
		/*TetrFirstOrder*/ int find_owner_tetr(CalcNode* node, float dx, float dy, float dz, bool debug);
		
		gsl_matrix *T;
		gsl_matrix *S;
		gsl_permutation *P;
		
	public:
		/*
		 * Constructor and destructor.
		 */
		TetrMeshFirstOrder();
		~TetrMeshFirstOrder();
		//void copyMesh(TetrMeshFirstOrder* src);
		
		int getTetrsNumber();
		int getTriangleNumber();
		
		void addTetr(TetrFirstOrder* tetr);
		/*
		 * Returns tetr by its index.
		 */
		TetrFirstOrder* getTetr(unsigned int index);
		
		TetrFirstOrder* getTetrByLocalIndex(unsigned int index);
		
		TriangleFirstOrder* getTriangle(int index);
		// FIXME should two functions belowe be moved outside this class?
		/*
		 * Creates tetrahedrons.
		 */
		void createTetrs(int number);
		
		void createTriangles(int number);
		
		void preProcessGeometry();
		
		float getRecommendedTimeStep();

		// Finds minimum h over mesh
		float getMinH();
		
		float getMaxH();
		
		float getAvgH();
		
		void doNextPartStep(float tau, int stage);
		
		int findTargetPoint(CalcNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
		void findBorderNodeNormal(int border_node_index, float* x, float* y, float* z, bool debug);
		void checkTopology(float tau);
		void interpolate(CalcNode* node, TetrFirstOrder* tetr);
		int prepare_node(CalcNode* cur_node, ElasticMatrix3D* elastic_matrix3d,
														float time_step, int stage,
														float* dksi, bool* inner, CalcNode* previous_nodes,
														float* outer_normal, int* ppoint_num);
		int find_nodes_on_previous_time_layer(CalcNode* cur_node, int stage,
														float dksi[], bool inner[], CalcNode previous_nodes[],
														float outer_normal[], int ppoint_num[]);

		void interpolateNode(int tetrInd, int prevNodeInd, CalcNode* previous_nodes);
	};
}
#endif