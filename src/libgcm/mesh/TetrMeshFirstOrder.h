#ifndef GCM_TETR_MESH_FIRST_ORDER_H_
#define GCM_TETR_MESH_FIRST_ORDER_H_

#include <algorithm>
#include <gsl/gsl_linalg.h>
#include "../node/ElasticNode.h"
#include "Mesh.h"
#include "../elem/TetrFirstOrder.h"
#include "../elem/TriangleFirstOrder.h"
#include "../Logging.h"
#include "../Exception.h"
#include "../Math.h"
#include "../util/AABB.h"
#include "../method/NumericalMethod.h"

using namespace gcm;

#define MapIter map<int, int>::const_iterator
#define STORAGE_OVERCOMMIT_RATIO 3.0

namespace gcm {
	/*
	 * Tetrahedral 1st order mesh.
	 */
	class TetrMeshFirstOrder: public Mesh {
		
	friend class VTKSnapshotWriter;
	friend class DataBus;
		
	protected:
		map<int, int> tetrsMap;
		map<int, int> nodesMap;
		/*
		 * List of mesh tetrahedrons.
		 */
		TetrFirstOrder* tetrs1;
		TriangleFirstOrder* border1;

		int triSizeInBytes;
		int tetrSizeInBytes;
		
		int numericalMethodOrder;
		/*
		 * List of mesh nodes.
		 */
		ElasticNode* nodes;
		ElasticNode* new_nodes;
		int nodesNumber;
		int nodesStorageSize;
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
		void create_outline();
		void verifyTetrahedraVertices ();
		void initNewNodes();
		
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
	
		/*TetrFirstOrder*/ int find_border_cross(ElasticNode* node, float dx, float dy, float dz, bool debug, float* cross);
		/*TetrFirstOrder*/ int find_border_cross(ElasticNode* node, float dx, float dy, float dz, bool debug, ElasticNode* cross);
		
		void calc_min_h();
		void calc_avg_h();
		void calc_max_h();
		void clearErrorFlags();
		// It MUST take into account mesh topology.
		// So, if the mesh will be second order, h = h / 2, etc
		float mesh_min_h;
		float mesh_avg_h;
		float mesh_max_h;

		USE_LOGGER;
		
		AABB outline;
		AABB expandedOutline;
		AABB syncedArea;
		AABB areaOfInterest;
		
		/*TetrFirstOrder*/ int expandingScanForPoint (ElasticNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
		/*TetrFirstOrder*/ int expandingScanForOwnerTetr(ElasticNode* node, float dx, float dy, float dz, bool debug);
		/*TetrFirstOrder*/ int fastScanForOwnerTetr(ElasticNode* node, float dx, float dy, float dz, bool debug);
		
		gsl_matrix *T;
		gsl_matrix *S;
		gsl_permutation *P;
		
	public:
		/*
		 * Constructor and destructor.
		 */
		TetrMeshFirstOrder();
		~TetrMeshFirstOrder();
		void copyMesh(TetrMeshFirstOrder* src);
		
		/*
		 * Returns number of nodes in mesh.
		 */
		int getNodesNumber();
		
		int getTetrsNumber();
		
		int getTriangleNumber();
		/*
		 * Returns node by its index.
		 */
		ElasticNode* getNode(int index);
		ElasticNode* getNewNode(int index);
		
		ElasticNode* getNodeByLocalIndex(int index);
		
		int getNodeLocalIndex(int index);
		
		void addNode(ElasticNode* node);
		void addTetr(TetrFirstOrder* tetr);
		/*
		 * Returns tetr by its index.
		 */
		TetrFirstOrder* getTetr(int index);
		
		TetrFirstOrder* getTetrByLocalIndex(int index);
		
		TriangleFirstOrder* getTriangle(int index);
		// FIXME should two functions belowe be moved outside this class?
		/*
		 * Creates nodes.
		 */
		void createNodes(int number);
		/*
		 * Creates tetrahedrons.
		 */
		void createTetrs(int number);
		
		void createTriangles(int number);
		/*
		 * Return arrays containing mesh nodes and tetrahedrons. Use carefully.
		 */
		//ElasticNode* getNodes();
		
		// TetrFirstOrder* getTetrs();
		
		//TriangleFirstOrder* getBorder();
		
		void preProcess();
		
		float getMaxPossibleTimeStep();
		float getMaxLambda();
		float getRecommendedTimeStep();

		// Finds minimum h over mesh
		float get_min_h();
		
		float get_max_h();
		
		float get_avg_h();
		
		void do_next_part_step(float tau, int stage);
		void move_coords(float tau);
		int proceed_rheology();
		
		/*TetrFirstOrder*/ int find_owner_tetr(ElasticNode* node, float dx, float dy, float dz, bool debug);
		/*TetrFirstOrder*/ int findTargetPoint(ElasticNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
		void find_border_node_normal(int border_node_index, float* x, float* y, float* z, bool debug);
		void printBorder();
		void setInitialState(Area* area, float* values);
		void setRheology(float la, float mu, float rho);
		void setRheology(float la, float mu, float rho, Area* area);
		void checkTopology(float tau);
		void interpolate(ElasticNode* node, TetrFirstOrder* tetr);
		inline AABB* getOutline()
		{
			return &outline;
		};
		
		void transfer(float x, float y, float z);
	};
}
#endif