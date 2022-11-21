#ifndef MaxwellViscosityCorrector_HPP
#define MaxwellViscosityCorrector_HPP
#include "libgcm/rheology/correctors/ICorrector.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include <cmath>

#include <iostream>

namespace gcm
{
        class MaxwellViscosityCorrector: public ICorrector
        {
            public:
            /**
             * Perform correction for the node
             *
             * @param node Node to perform correction on
             */
            void correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step);
        };
}
#endif /* MaxwellViscosityCorrector_HPP */
