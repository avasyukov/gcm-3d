#ifndef GCM_MESH_H_
#define GCM_MESH_H_

#include <string>

// FIXME
#include "../elem/TetrFirstOrder.h"
#include "../node/ElasticNode.h"

#include "../Interfaces.h"
#include "../node/Node.h"

using namespace std;
using namespace gcm;

namespace gcm {
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
		
		// FIXME;
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
		/*
		 * Returns node by its index.
		 */
		virtual Node* getNode(int index) = 0;
		
		// FIXME
		virtual int findTargetPoint(ElasticNode* node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint) = 0;
		virtual void find_border_node_normal(int border_node_index, float* x, float* y, float* z, bool debug) = 0;
		virtual void printBorder() = 0;
		void update_current_time(float time_step);
		float get_current_time();
		virtual void setInitialState(Area* area, float* values) = 0;
		virtual TetrFirstOrder* getTetr(int i) = 0;
		virtual float getMaxPossibleTimeStep() = 0;
		virtual float getRecommendedTimeStep() = 0;
		virtual float getMaxLambda() = 0;
		virtual void do_next_part_step(float tau, int stage) = 0;
		// FIXME
		virtual void addNode(ElasticNode* node) = 0;
		// FIXME
		virtual void addTetr(TetrFirstOrder* tetr) = 0;
		virtual void setRheology(float la, float mu, float rho) = 0;
		virtual void setRheology(float la, float mu, float rho, Area* area) = 0;
		virtual void checkTopology(float tau) = 0;
		virtual	void transfer(float x, float y, float z) = 0;

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
