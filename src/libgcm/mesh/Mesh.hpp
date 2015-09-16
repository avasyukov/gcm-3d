#ifndef GCM_MESH_H_
#define GCM_MESH_H_

#include <unordered_map>
#include <string>
#include <algorithm>
#include <functional>

#include "libgcm/util/AABB.hpp"
#include "libgcm/util/areas/Area.hpp"
#include "libgcm/rheology/RheologyCalculator.hpp"
#include "libgcm/node/Node.hpp"
#include "libgcm/snapshot/SnapshotWriter.hpp"

#include "libgcm/failure/FailureModel.hpp"

#define STORAGE_OVERCOMMIT_RATIO 1.0
#define STORAGE_ONDEMAND_GROW_RATE 1.25


typedef std::unordered_map<int, int>::const_iterator MapIter;

namespace gcm {
    class CalcNode;
    class Body;
    /*
     * Base class for all meshes
     */
    class Mesh {
    private:
        /*
         * Mesh type. Used in runtime to determine mesh type.
         */
        std::string type;
        /*
         * Calculatable flag.
         */
        bool calc;
        /*
         * Mesh id.
         */
        std::string id;

    protected:
        /*
         * Body
         */
        Body* body;

        /*
         * You need to maintain these outlines
         */
        // TODO - implement some check that these outlines are maintained really
        AABB outline;
        AABB expandedOutline;
        AABB syncedArea;
        AABB areaOfInterest;

        std::string numericalMethodType;
        int numericalMethodOrder;

        /*
         * List of mesh nodes.
         */
        std::vector<CalcNode> nodes;
        std::vector<CalcNode> new_nodes;
        std::unordered_map<int, int> nodesMap;
        int nodesNumber;
        int nodesStorageSize;

        bool movable;

        void initNewNodes();

        USE_LOGGER;

        virtual const SnapshotWriter& getSnaphotter() const = 0;
        virtual const SnapshotWriter& getSnapshotter2() const;
        virtual const SnapshotWriter& getDumper() const = 0;

    public:
        /*
         * Constructor and destructor.
         */
        Mesh();
        // See http://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
        Mesh(std::string _type);

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
		
		virtual float getAvgH() = 0;
        /*
         * Performs mesh pre-processing
         * It is called
         *    (a) either after the mesh was created
         *    (b) or after mesh nodes were rebalanced
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
        
        /*
         * Takes vector from (x; y; z) with length (dx; dy; dz) and
         * finds its intersection with mesh border.
         * It's a separate function since (x; y; z) is outside of the mesh, 
         * and we neither have 'origin' node nor exact 'target' node position.
         * Returns 'true' if vector intersects mesh border and target node interpolated successfully.
         * Returns 'false' if vector does not intersect mesh border.
         */
        virtual bool interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node) = 0;

        virtual void findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug) = 0;


        std::string snapshot(int number);
        std::string dump(int number);

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
        int getNodeLocalIndex(int index) const;
        CalcNode& getNodeByLocalIndex(unsigned int index);
        void addNode(CalcNode& node);

        /*
         * Sets mesh id.
         */
        void setId(std::string id);
        /*
         * Returns mesh id.
         */
        std::string getId() const;
        /*
         * Returns type of mesh.
         */
        std::string getType();
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
        void setBody(Body *body);

        void setBodyNum(unsigned char id);
        /*
         * Returns mesh body.
         */
        Body* getBody();

        virtual void createOutline();
        AABB getOutline();
        const AABB& getExpandedOutline() const;

        void preProcess();

        void setInitialState(Area* area, float* values);
        void setInitialState(Area* area, std::function<void(CalcNode& node)> setter);
		void setBorderCondition(Area* area, unsigned int num);
		void setContactCondition(Area* area, unsigned int num);
        void setRheology(unsigned char matId);
        void setRheology(unsigned char matId, Area* area);

        virtual void transfer(float x, float y, float z);
		void scale(float x0, float y0, float z0, 
				float scaleX, float scaleY, float scaleZ);
        void applyRheology(RheologyCalculator* rc);
        void clearContactState();
        void clearNodesState();
        void processStressState();
        void processMaterialFailure(FailureModel* failureModel, const float tau);
        void applyCorrectors();
        virtual void moveCoords(float tau);

        float getMaxEigenvalue();
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
