#ifndef GCM_BASIC_CUBIC_MESH_H_
#define GCM_BASIC_CUBIC_MESH_H_

#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Exception.hpp"

#include "libgcm/interpolator/LineFirstOrderInterpolator.hpp"
#include "libgcm/interpolator/LineSecondOrderInterpolator.hpp"


#include <utility>
#include <algorithm>

namespace gcm
{
    class CalcNode;

    class BasicCubicMesh: public Mesh {

    // Hopefully we will never need these 'friends' back
/*    friend class VTKSnapshotWriter;
    friend class DataBus;
    friend class CollisionDetector;
    friend class BruteforceCollisionDetector;
*/

    protected:
        LineFirstOrderInterpolator* interpolator1;
		LineSecondOrderInterpolator* interpolator2;

        void logMeshStats();
        void calcMinH();
        void preProcessGeometry() override;

        virtual int findNeighbourPoint(CalcNode& node, float dx, float dy, float dz, 
		                       bool debug, float* coords, bool* innerPoint);


        // Evaluates sizes of basic element along each axis
        void calcSpacialSteps();

        // Minimal size of basic element
        float meshH;

        // Sizes of basic element along axis
        float hx, hy, hz;

        // Number of cubes along axis
        int numX, numY, numZ;

        USE_LOGGER;

    public:
        BasicCubicMesh();
		BasicCubicMesh(std::string _type);
        ~BasicCubicMesh();

        float getRecommendedTimeStep();
        float getMinH();
		float getAvgH() override;
        void doNextPartStep(float tau, int stage) override;
        void checkTopology(float tau);

        void findNearestsNodes(const vector3r& coords, int N, std::vector< std::pair<int,float> >& result);

        void findNearestsNodesForNode(const vector3r& coords, std::vector<int>& result);

        void findBorderNodeNormal(const CalcNode& node, float* x, float* y, float* z, bool debug) override;

        bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                CalcNode& targetNode, bool& isInnerPoint) override;

        bool interpolateNode(CalcNode& node) override;

        bool interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node) override;
        bool interpolateBorderNode_old(real x, real y, real z,
                                        real dx, real dy, real dz, CalcNode& node);

        void setNumX(int _numX);
        void setNumY(int _numY);
        void setNumZ(int _numZ);

        int getNumX() const;
        int getNumY() const;
        int getNumZ() const;
        float getH() const;

        float getHx() const;
        float getHy() const;
        float getHz() const;

        virtual const SnapshotWriter& getSnaphotter() const override;
        virtual const SnapshotWriter& getDumper() const override;
    };
}
#endif
