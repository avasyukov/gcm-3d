#ifndef GCM_INTERPOLATION_FIXED_AXIS_H
#define    GCM_INTERPOLATION_FIXED_AXIS_H

#include "libgcm/mesh/tetr/TetrMeshFirstOrder.h"
#include "libgcm/mesh/tetr/TetrMeshSecondOrder.h"
#include "libgcm/method/NumericalMethod.h"
#include "libgcm/mesh/Mesh.h"
#include "libgcm/util/AnisotropicMatrix3D.h"
#include "libgcm/util/ElasticMatrix3D.h"
#include "libgcm/util/Types.h"
#include "libgcm/node/CalcNode.h"
#include "libgcm/Logging.h"
#include "libgcm/Exception.h"


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

