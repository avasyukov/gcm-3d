#ifndef GCM_TETR_MESH_FIRST_ORDER_H_
#define GCM_TETR_MESH_FIRST_ORDER_H_

#include <unordered_map>
#include <algorithm>
#include <gsl/gsl_linalg.h>
#include "Mesh.h"
#include "../elem/TetrFirstOrder.h"
#include "../elem/TriangleFirstOrder.h"
#include "../Logging.h"
#include "../Exception.h"
#include "../Math.h"
#include "../util/AABB.h"
#include "../method/NumericalMethod.h"

using namespace gcm;

#define MapIter unordered_map<int, int>::const_iterator
#define STORAGE_OVERCOMMIT_RATIO 1.0
#define STORAGE_ONDEMAND_GROW_RATE 1.25

namespace gcm {
	class CalcNode;
	/*
	 * Tetrahedral 1st order mesh.
	 */
	class TetrMeshFirstOrder: public Mesh {
		
	friend class VTKSnapshotWriter;
	friend class DataBus;
	friend class CollisionDetector;
	friend class BruteforceCollisionDetector;
		
	protected:
		unordered_map<int, int> tetrsMap;
		unordered_map<int, int> nodesMap;
		
		// Cache for characteristics hits
		bool charactCacheAvailable();
		bool checkCharactCache(CalcNode* node, float dx, float dy, float dz, int& tetrNum);
		void updateCharactCache(CalcNode* node, float dx, float dy, float dz, int tetrNum);
		int getCharactCacheIndex(CalcNode* node, float dx, float dy, float dz);
		unordered_map<int, int> charactCache[12];
		/*map<int, int> charactCacheXps;
		map<int, int> charactCacheXpl;
		map<int, int> charactCacheXms;
		map<int, int> charactCacheXml;
		map<int, int> charactCacheYps;
		map<int, int> charactCacheYpl;
		map<int, int> charactCacheYms;
		map<int, int> charactCacheYml;
		map<int, int> charactCacheZps;
		map<int, int> charactCacheZpl;
		map<int, int> charactCacheZms;
		map<int, int> charactCacheZml;*/
		unsigned long long cacheHits;
		unsigned long long cacheMisses;
		
		/*
		 * List of mesh tetrahedrons.
		 */
		vector<TetrFirstOrder> tetrs1;
		vector<TriangleFirstOrder> border1;

		int numericalMethodOrder;
		/*
		 * List of mesh nodes.
		 */
		vector<CalcNode> nodes;
		vector<CalcNode> new_nodes;
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
	
		/*TetrFirstOrder*/ int find_border_cross(CalcNode* node, float dx, float dy, float dz, bool debug, float* cross);
		/*TetrFirstOrder*/ int find_border_cross(CalcNode* node, float dx, float dy, float dz, bool debug, CalcNode* cross);
		
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
		void copyMesh(TetrMeshFirstOrder* src);
		
		/*
		 * Returns number of nodes in mesh.
		 */
		int getNodesNumber();
		
		int getTetrsNumber();
		
		int getTriangleNumber();
		
		AABB getOutline();
		AABB getExpandedOutline();
		/*
		 * Returns node by its index.
		 */
		CalcNode* getNode(int index);
		CalcNode* getNewNode(int index);
		
		CalcNode* getNodeByLocalIndex(int index);
		
		int getNodeLocalIndex(int index);
		
		void addNode(CalcNode* node);
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
		//CalcNode* getNodes();
		
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
		
		/*TetrFirstOrder*/ int findTargetPoint(CalcNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
		void find_border_node_normal(int border_node_index, float* x, float* y, float* z, bool debug);
		void printBorder();
		void setInitialState(Area* area, float* values);
		void setRheology(unsigned char matId);
		void setRheology(unsigned char matId, Area* area);
		void checkTopology(float tau);
		void interpolate(CalcNode* node, TetrFirstOrder* tetr);
		
		void transfer(float x, float y, float z);
		void setBodyNum(unsigned char id);
		int getNumberOfLocalNodes();
		void clearContactState();
		
		void applyRheology(RheologyCalculator* rc);
	};
}
#endif
