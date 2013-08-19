#ifndef GCM_MESH_H_
#define GCM_MESH_H_

#include <string>

// FIXME - it's bad to have 'firstOrder' specifics here
#include "../elem/TetrFirstOrder.h"
#include "../elem/TriangleFirstOrder.h"
//#include "../node/CalcNode.h"

#include "../Interfaces.h"
#include "../node/Node.h"

using namespace std;
using namespace gcm;

namespace gcm {
	class CalcNode;
	class Area;
	/*
	 * Base class for all meshes
	 */
	class Mesh {
	private:
		/*
		 * Mesh type. Used in runtime to determine mesh type.
		 */
		string type;
		/*
		 * Calculatable flag.
		 */
		bool calc;
		/*
		 * Mesh id.
		 */
		string id;
	protected:
		/*
		 * Body
		 */
		IBody* body;
		
		// FIXME - do we need it in mesh?
		float current_time;
	public:
		/*
		 * Constructor and destructor.
		 */
		Mesh();
		virtual ~Mesh();
		/*
		 * Returns type of mesh.
		 */
		string getType();
		/*
		 * Returns number of nodes in mesh.
		 */
		virtual int getNodesNumber() = 0;
		
		virtual AABB getOutline() = 0;
		/*
		 * Returns node by its index.
		 */
		virtual CalcNode* getNode(int index) = 0;
		virtual CalcNode* getNewNode(int index) = 0;
		virtual CalcNode* getNodeByLocalIndex(int index) = 0;
		virtual TetrFirstOrder* getTetr(int index) = 0;
		virtual TetrFirstOrder* getTetrByLocalIndex(int index) = 0;
		virtual TriangleFirstOrder* getTriangle(int index) = 0;
		
		virtual void createNodes(int number) = 0;
		virtual void createTetrs(int number) = 0;
		virtual void createTriangles(int number) = 0;
		
		// FIXME - do we need it in Mesh
		virtual void addNode(CalcNode* node) = 0;
		// FIXME - it's bad to have firstOrder specific here
		virtual void addTetr(TetrFirstOrder* tetr) = 0;
		
		// FIXME - do we need it in pure virtual Mesh?
		virtual int findTargetPoint(CalcNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint) = 0;
		virtual void find_border_node_normal(int border_node_index, float* x, float* y, float* z, bool debug) = 0;
		virtual void printBorder() = 0;
		void update_current_time(float time_step);
		float get_current_time();
		virtual void setInitialState(Area* area, float* values) = 0;
		virtual float getMaxPossibleTimeStep() = 0;
		virtual float getRecommendedTimeStep() = 0;
		virtual float getMaxLambda() = 0;
		virtual void do_next_part_step(float tau, int stage) = 0;
		virtual void setRheology(unsigned char matId) = 0;
		virtual void setRheology(unsigned char matId, Area* area) = 0;
		virtual void checkTopology(float tau) = 0;
		virtual	void transfer(float x, float y, float z) = 0;
		virtual void setBodyNum(unsigned char id) = 0;
		virtual void clearContactState() = 0;

		/*
		 * Sets mesh id.
		 */
		void setId(string id);
		/*
		 * Returns mesh id.
		 */
		string getId();
		/*
		 * Sets calc flag.
		 */
		void setCalc(bool calc);
		/*
		 * Returns calc flag.
		 */
		bool getCalc();
		/*
		 * Sets body mesh belongs to.
		 */
		void setBody(IBody *body);
		/*
		 * Returns mesh body.
		 */
		IBody* getBody();		
	};

	/*
	 * Constants for implemented mesh types. Use >100 when
	 * implementing your custom mesh type.
	 */
	static const int TYPE_TETRAHEDRAL_1ST_ORDER = 1;
	static const int TYPE_TETRAHEDRAL_2ND_ORDER = 2;
}
#endif
