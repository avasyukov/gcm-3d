#ifndef GCM_INTERPOLATION_FIXED_AXIS_H
#define    GCM_INTERPOLATION_FIXED_AXIS_H

#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"
#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/method/NumericalMethod.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/util/AnisotropicMatrix3D.hpp"
#include "libgcm/util/ElasticMatrix3D.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Exception.hpp"


namespace gcm
{

    class InterpolationFixedAxis : public NumericalMethod {
    public:
        InterpolationFixedAxis();
        virtual ~InterpolationFixedAxis();
        int getNumberOfStages();
        void doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh);
        string getType();
    protected:
        int prepare_node(CalcNode& cur_node, RheologyMatrix3D& rheologyMatrix,
                float time_step, int stage, Mesh* mesh,
                float* dksi, bool* inner, vector<CalcNode>& previous_nodes,
                float* outer_normal);
        int find_nodes_on_previous_time_layer(CalcNode& cur_node, int stage, Mesh* mesh,
                float dksi[], bool inner[], vector<CalcNode>& previous_nodes,
                float outer_normal[]);

        USE_LOGGER;
    };
}

#endif    /* GCM_INTERPOLATION_FIXED_AXIS_H */

