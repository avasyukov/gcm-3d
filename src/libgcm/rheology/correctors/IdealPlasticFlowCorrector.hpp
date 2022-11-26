#ifndef IDEALPLASTICFLOWCORRECTOR_HPP
#define IDEALPLASTICFLOWCORRECTOR_HPP 
#include "libgcm/rheology/correctors/ICorrector.hpp"
#include <cmath>
#include "libgcm/mesh/Mesh.hpp"

#include <iostream>

namespace gcm
{
        class IdealPlasticFlowCorrector: public ICorrector
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
#endif /* IDEALPLASTICFLOWCORRECTOR_HPP */
