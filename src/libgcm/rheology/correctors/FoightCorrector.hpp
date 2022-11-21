#ifndef FoightCorrector_HPP
#define FoightCorrector_HPP
#include "libgcm/rheology/correctors/ICorrector.hpp"
#include <cmath>
#include "libgcm/Logging.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/mesh/Mesh.hpp"

#include <iostream>

namespace gcm
{
        class FoightCorrector: public ICorrector
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
#endif /* FoightCorrector_HPP */
