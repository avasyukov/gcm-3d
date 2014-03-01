#ifndef MARKERED_MESH_H_
#define MARKERED_MESH_H_

#include <vector>

#include "mesh/Mesh.h"
#include "node/CalcNode.h"
#include "node/Node.h"

using namespace std;

namespace gcm {
	typedef unsigned int NeighbourCells[26];
	typedef unsigned int CommonPoints[4];
	typedef unsigned int CellPoints[8];

	typedef struct {
		union {
			unsigned int cells[8];
			struct {
				unsigned int c000;
				unsigned int c010;
				unsigned int c100;
				unsigned int c110;
				unsigned int c001;
				unsigned int c011;
				unsigned int c101;
				unsigned int c111;
			};
		};
	} PointCells;

	class MarkeredMesh: public Mesh {
	protected:
		AABB meshOutline;
		LineFirstOrderInterpolator* interpolator;
		unsigned int points_num[3];
		unsigned int cells_num[3];
		vector<bool> cellsInnerFlags;
		vector<Node> markers;
		float h[3];
		void logMeshStats();
		void calcMinH();
		void preProcessGeometry();

		USE_LOGGER;
	public:
		MarkeredMesh();
		float getRecommendedTimeStep();
		float getMinH();
		void doNextPartStep(float tau, int stage);
		void checkTopology(float tau);

		void findBorderNodeNormal(int border_node_index, float* x, float* y, float* z, bool debug);
		bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
								CalcNode& targetNode, bool& isInnerPoint);


		void setNumberOfCells(unsigned int num1, unsigned int num2, unsigned int num3);
		void getNumberOfCells(int &num1, int &num2, int &num3);
		void getNumberOfPoints(int &num1, int &num2, int &num3);
		unsigned int getTotalNumberOfCells();

		void getCellAABB(unsigned int num, AABB& aabb);
		int getPointNumber(unsigned int i, unsigned int j, unsigned int k);
		int getCellNumber(unsigned int i, unsigned int j, unsigned int k);

		void setCellInnerFlag(unsigned int num, bool flag);
		bool getCellInnerFlag(unsigned int num);

		int generateMarkers();

		void reconstructInnerFlags();

		long getCellForPoint(float x, float y, float z);

		void createOutline();

		unsigned int getCellNeighbours(unsigned int n, NeighbourCells &cells);

		void getCellIndexes(unsigned int num, unsigned int &i1, unsigned int &i2, unsigned int &i3);
		void getPointIndexes(unsigned int num, unsigned int &i1, unsigned int &i2, unsigned int &i3);

		const vector<Node>& getMarkers();

		const AABB& getMeshOutline();

		void reconstructBorder();

		unsigned int getCellsCommonPoints(unsigned int c1, unsigned int c2, CommonPoints &pts);

		void getCellPoints(unsigned int n, CellPoints &pts);

		void markUnusedNodes();

		void moveMarkers(float dt);
	};
}


#endif
