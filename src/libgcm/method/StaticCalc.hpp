#ifndef GCM_StaticCalc_H
#define    GCM_StaticCalc_H

#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"
#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/method/NumericalMethod.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/Exception.hpp"

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>

namespace gcm
{
    struct Constraint
    {
        enum Type
        {
            UX = 1 << 0,
            UY = 1 << 1,
            UZ = 1 << 2,
            UXY = UX | UY,
            UXZ = UX | UZ,
            UYZ = UY | UZ,
            UXYZ = UX | UY | UZ
        };
        int node;
        Type type;
    };

    class StaticCalc : public NumericalMethod {
    public:
        StaticCalc();
        virtual ~StaticCalc();
        int getNumberOfStages();
        void doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh);
        std::string getType();
    protected:
        void SetConstraints(Eigen::SparseMatrix<float>::InnerIterator& it, int index);
        void ApplyConstraints(Eigen::SparseMatrix<float>& K, const std::vector<Constraint>& constraints);

        USE_LOGGER;
    };
}

#endif    /* GCM_StaticCalc_H */

