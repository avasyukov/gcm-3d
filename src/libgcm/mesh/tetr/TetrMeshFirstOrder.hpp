#ifndef GCM_TETR_MESH_FIRST_ORDER_H_
#define GCM_TETR_MESH_FIRST_ORDER_H_

#include <unordered_map>
#include <algorithm>

#include <gsl/gsl_linalg.h>

#include "libgcm/mesh/tetr/TetrMesh.hpp"
#include "libgcm/elem/TetrFirstOrder.hpp"
#include "libgcm/elem/TriangleFirstOrder.hpp"
#include "libgcm/util/AABB.hpp"
#include "libgcm/method/NumericalMethod.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/Math.hpp"
#include "libgcm/interpolator/TetrFirstOrderInterpolator.hpp"
#include "libgcm/snapshot/VTKSnapshotWriter.hpp"

using namespace gcm;
using namespace std;

namespace gcm
{
    class CalcNode;
    /*
     * Tetrahedral 1st order mesh.
     */
    class TetrMeshFirstOrder: public TetrMesh {

    friend class VTKSnapshotWriter;
    friend class DataBus;
    friend class CollisionDetector;
    friend class BruteforceCollisionDetector;

    private:
        TetrFirstOrderInterpolator* interpolator;

    protected:
        unordered_map<int, int> tetrsMap;

        // Cache for characteristics hits
        bool charactCacheAvailable();
        bool checkCharactCache(CalcNode& node, float dx, float dy, float dz, int& tetrNum);
        void updateCharactCache(CalcNode& node, float dx, float dy, float dz, int tetrNum);
        int getCharactCacheIndex(CalcNode& node, float dx, float dy, float dz);
        unordered_map<int, int> charactCache[18];
        unsigned long long cacheHits;
        unsigned long long cacheMisses;

        /*
         * List of mesh tetrahedrons.
         */
        vector<TetrFirstOrder> tetrs1;
        vector<TriangleFirstOrder> border1;

        int tetrsNumber;
        int tetrsStorageSize;
        int faceNumber;
        int faceStorageSize;

        vector< vector<int> > volumeElements;
        vector< vector<int> > borderElements;
        vector<int>& getVolumeElementsForNode(int index);
        vector<int>& getBorderElementsForNode(int index);

        void build_surface_reverse_lookups();
        void build_volume_reverse_lookups();
        void build_border();
        void check_numbering();
        void check_outer_normals();
        void check_unused_nodes();
        void verifyTetrahedraVertices ();

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

        void calcMinH();
        void calcAvgH();
        void calcMaxH();
        // It MUST take into account mesh topology.
        // So, if the mesh will be second order, h = h / 2, etc
        float mesh_min_h;
        float mesh_avg_h;
        float mesh_max_h;

        USE_LOGGER;

        int expandingScanForOwnerTetr(CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
        int fastScanForOwnerTetr(CalcNode& node, float dx, float dy, float dz, bool debug);
        int findOwnerTetr(CalcNode& node, float dx, float dy, float dz, bool debug, float* coords, bool* innerPoint);
        bool isInnerPoint(CalcNode& node, float dx, float dy, float dz, bool debug);

        gsl_matrix *T;
        gsl_matrix *S;
        gsl_permutation *P;

    public:
        /*
         * Constructor and destructor.
         */
        TetrMeshFirstOrder();
        virtual ~TetrMeshFirstOrder();
        //void copyMesh(TetrMeshFirstOrder* src);

        int getTetrsNumber();
        int getTriangleNumber();

        void addTetr(TetrFirstOrder& tetr);
        /*
         * Returns tetr by its index.
         */
        TetrFirstOrder& getTetr(unsigned int index);

        TetrFirstOrder& getTetrByLocalIndex(unsigned int index);

        bool hasTetr(unsigned int index);

        TriangleFirstOrder& getTriangle(int index);
        // FIXME should two functions belowe be moved outside this class?
        /*
         * Creates tetrahedrons.
         */
        void createTetrs(int number);

        void createTriangles(int number);

        void preProcessGeometry();

        float getRecommendedTimeStep();

        // Finds minimum h over mesh
        float getMinH();

        float getMaxH();

        float getAvgH();

        void doNextPartStep(float tau, int stage);

        void findBorderNodeNormal(unsigned int border_node_index, float* x, float* y, float* z, bool debug);
        void checkTopology(float tau);

        bool interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                CalcNode& targetNode, bool& isInnerPoint);

        bool interpolateNode(CalcNode& node);
		
        bool interpolateBorderNode(real x, real y, real z, 
                                real dx, real dy, real dz, CalcNode& node);

        virtual const SnapshotWriter& getSnaphotter() const override;
        virtual const SnapshotWriter& getDumper() const override;
    };
}
#endif
