#ifndef GCM_BASIC_CUBIC_MESH_H_
#define GCM_BASIC_CUBIC_MESH_H_

#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Exception.hpp"

#include "libgcm/interpolator/LineFirstOrderInterpolator.hpp"
#include "libgcm/interpolator/LineSecondOrderInterpolator.hpp"

using namespace gcm;

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

    private:
        LineFirstOrderInterpolator* interpolator1;
		LineSecondOrderInterpolator* interpolator2;

    protected:
        void logMeshStats();
        void calcMinH();
        void preProcessGeometry();

        int findNeighbourPoint(CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);

        float meshH;

        USE_LOGGER;

    public:
        BasicCubicMesh();
        ~BasicCubicMesh();

        float getRecommendedTimeStep();
        float getMinH();
        void doNextPartStep(float tau, int stage);
        void checkTopology(float tau);

        void findBorderNodeNormal(int border_node_index, float* x, float* y, float* z, bool debug);

        bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                CalcNode& targetNode, bool& isInnerPoint);

        bool interpolateNode(CalcNode& node);

        bool interpolateBorderNode(gcm_real x, gcm_real y, gcm_real z, 
                                gcm_real dx, gcm_real dy, gcm_real dz, CalcNode& node);
    };
}
#endif
