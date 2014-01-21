#ifndef GCM_TETR_MESH_SECOND_ORDER_H_
#define GCM_TETR_MESH_SECOND_ORDER_H_

#include "TetrMeshFirstOrder.h"
#include "../../elem/TetrSecondOrder.h"
#include "../../elem/TriangleSecondOrder.h"

using namespace std;
using namespace gcm;

typedef std::pair <int, int> IntPair;

namespace gcm {
	/*
	 * Tetrahedral 2nd order mesh.
	 */
	class TetrMeshSecondOrder: public TetrMeshFirstOrder {
		
	friend class VTK2SnapshotWriter;
	friend class DataBus;
	friend class CollisionDetector;
	friend class BruteforceCollisionDetector;
		
	private:
		/*
		 * List of mesh tetrahedrons.
		 */
		vector<TetrSecondOrder> tetrs2;
		vector<TriangleSecondOrder> border2;
		bool secondOrderNodesAreGenerated;
		int firstOrderNodesNumber;
		int secondOrderNodesNumber;
		
	protected:
		int countSecondOrderNodes(TetrMeshFirstOrder* src);
		void generateSecondOrderNodes();
		void fillSecondOrderNode(CalcNode* newNode, int nodeIdx1, int nodeIdx2);
		
		void verifyTetrahedraVertices();
		void build_volume_reverse_lookups();
		void build_surface_reverse_lookups();
		void build_first_order_border();
		void generateSecondOrderBorder();
		void moveSecondOrderNode(int nodeIdx, int nodeIdx1, int nodeIdx2);
		
	public:
		TetrMeshSecondOrder();
		~TetrMeshSecondOrder();
		void copyMesh(TetrMeshFirstOrder* src);
		void copyMesh2(TetrMeshSecondOrder* src);
		void createTetrs(int number);
		void createTriangles(int number);
		/*
		 * Returns tetr by its index.
		 */
		TetrFirstOrder* getTetr(int index);
		TetrSecondOrder* getTetr2(int index);
		
		TetrFirstOrder* getTetrByLocalIndex(int index);
		TetrSecondOrder* getTetr2ByLocalIndex(int index);
		
		void addTetr(TetrFirstOrder* tetr);
		void addTetr2(TetrSecondOrder* tetr);
		void rebuildMaps();
		
		TriangleFirstOrder* getTriangle(int index);
		TriangleSecondOrder* getTriangle2(int index);

		void preProcess();
		void move_coords(float tau);
	};
}
#endif