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

        int findNeighbourPoint(CalcNode& node, float dx, float dy, float dz, 
		                       bool debug, float* coords, bool* innerPoint);

        void findNearestsNodes(const vector3r& coords, int N, std::vector< std::pair<int,float> >& result);

        float meshH;

        // Number of cubes along axis
        int numX, numY, numZ;

        USE_LOGGER;

    public:
        BasicCubicMesh();
        ~BasicCubicMesh();

        float getRecommendedTimeStep();
        float getMinH();
		float getAvgH() override;
        void doNextPartStep(float tau, int stage) override;
        void checkTopology(float tau);

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

        virtual const SnapshotWriter& getSnaphotter() const override;
        virtual const SnapshotWriter& getDumper() const override;
    };
}
#endif
