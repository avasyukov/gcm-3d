#ifndef GCM_MESH_H_
#define GCM_MESH_H_

#include <unordered_map>
#include <string>

#include "util/AABB.h"
#include "util/RheologyMatrix3D.h"
#include "util/areas/Area.h"
#include "Interfaces.h"
#include "node/Node.h"

#define STORAGE_OVERCOMMIT_RATIO 1.0
#define STORAGE_ONDEMAND_GROW_RATE 1.25

using namespace std;
using namespace gcm;

namespace gcm {
	class CalcNode;
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
		
		/*
		 * You need to maintain these outlines
		 */
		// TODO - implement some check that these outlines are maintained really
		AABB outline;
		AABB expandedOutline;
		AABB syncedArea;
		AABB areaOfInterest;
		
		string numericalMethodType;
		int numericalMethodOrder;
		string snapshotWriterType;
		string dumpWriterType;
		
		// FIXME - do we need it in mesh?
		float current_time;
		
		/*
		 * List of mesh nodes.
		 */
		vector<CalcNode> nodes;
		vector<CalcNode> new_nodes;
		unordered_map<int, int> nodesMap;
		int nodesNumber;
		int nodesStorageSize;
		
		bool movable;
		
		void initNewNodes();
		
		USE_LOGGER;
		
	public:
		/*
		 * Constructor and destructor.
		 */
		Mesh();
		// See http://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
		virtual ~Mesh();
		
		
		// Virtual functions to be implemented by children classes
		
		/*
		 * Should return time step that is considered ideal for the mesh.
		 * It may be courant step or whatever.
		 * The simpliest option is to return just getMaxPossibleTimeStep() 
		 */ 
		virtual float getRecommendedTimeStep() = 0;
		
		/*
		 * Performs the next part step using time step 'tau' and stage number 'stage'.
		 * Most probably just calling defaultNextPartStep(tau, stage) will work.
		 */
		virtual void doNextPartStep(float tau, int stage) = 0;
		
		/*
		 * Calculates and stores minimum h over mesh.
		 * Minimum h is required very often, so we are to pre-calculate it.
		 */
		virtual void calcMinH() = 0;
		
		/*
		 * Returns pre-calculated minumum h
		 */
		virtual float getMinH() = 0;
		
		/*
		 * Performs mesh pre-processing
		 * It is called 
		 *	(a) either after the mesh was created 
		 *	(b) or after mesh nodes were rebalanced
		 */
		virtual void preProcessGeometry() = 0;
		
		/*
		 * It is called before each time step.
		 * You are supposed to perform necessary mesh operations here. Examples:
		 * - rethink areaOfInterest, syncedArea, expandedOutline, etc
		 * - optimize deformed mesh
		 * Most probably, it should be just left blank for non-moving meshes.
		 */
		virtual void checkTopology(float tau) = 0;
		
		/*
		 * Logs major mesh stats (obviously, it's specific for each mesh type)
		 */
		virtual void logMeshStats() = 0;
		
		/*
		 * Interpolates node that is (dx; dy; dz) from 'origin'.
		 * Return value - is the node was interpolated (as internal volume node or border cross).
		 * Returns interpolated results in 'targetNode'.
		 * Sets 'isInnerPoint' flag.
		 * If returns 'false', targetNode is undefined.
		 */
		virtual bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug, 
								CalcNode& targetNode, bool& isInnerPoint) = 0;
		
		/*
		 * Interpolates given point, returns values in the node itself.
		 * Returns 'true' if interpolated successfully 
		 * and 'false' if the node can not be interpolated with this mesh.
		 */
		virtual bool interpolateNode(CalcNode& node) = 0;
		
		virtual void findBorderNodeNormal(int border_node_index, float* x, float* y, float* z, bool debug) = 0;
		
		
		void snapshot(int number);
		void dump(int number);
		
		// We place all these functions into generic mesh to simplify children classes.
		// It allows to remove nodesMap complexity (required by parallel impl) from children classes.
		// We do believe that all children classes will use the same node storage.
		// If it's not the case, we need to convert these functions into virtual.
		int getNodesNumber();
		int getNumberOfLocalNodes();
		void createNodes(int number);
		bool hasNode(int index);
		CalcNode& getNode(int index);
		CalcNode& getNewNode(int index);
		int getNodeLocalIndex(int index);
		CalcNode& getNodeByLocalIndex(unsigned int index);
		void addNode(CalcNode& node);
		
		/*
		 * Sets mesh id.
		 */
		void setId(string id);
		/*
		 * Returns mesh id.
		 */
		string getId();
		/*
		 * Returns type of mesh.
		 */
		string getType();
		/*
		 * Sets calc flag.
		 */
		void setCalc(bool calc);
		/*
		 * Returns calc flag.
		 */
		bool getCalc();
		
		void setMovable(bool movable);
		bool getMovable();
		
		/*
		 * Sets body mesh belongs to.
		 */
		void setBody(IBody *body);
		
		void setBodyNum(unsigned char id);
		/*
		 * Returns mesh body.
		 */
		IBody* getBody();
		
		void update_current_time(float time_step);
		float get_current_time();
		
		virtual void createOutline();
		AABB getOutline();
		AABB getExpandedOutline();
		
		void preProcess();
		
		void setInitialState(Area* area, float* values);
		void setRheology(unsigned char matId);
		void setRheology(unsigned char matId, Area* area);
		
		void transfer(float x, float y, float z);
		void applyRheology(RheologyCalculator* rc);
		void clearContactState();
		void clearNodesState();
		void processStressState();
		void processCrackState();
		void processCrackResponse();
		void moveCoords(float tau);
		
		float getMaxLambda();
		float getMaxPossibleTimeStep();
		
		void defaultNextPartStep(float tau, int stage);
	};

	/*
	 * Constants for implemented mesh types. Use >100 when
	 * implementing your custom mesh type.
	 */
	static const int TYPE_TETRAHEDRAL_1ST_ORDER = 1;
	static const int TYPE_TETRAHEDRAL_2ND_ORDER = 2;
}
#endif
