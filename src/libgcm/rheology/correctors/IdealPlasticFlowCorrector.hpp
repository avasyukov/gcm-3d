#ifndef IDEALPLASTICFLOWCORRECTOR_HPP
#define IDEALPLASTICFLOWCORRECTOR_HPP 
#include "libgcm/rheology/correctors/ICorrector.hpp"
#include <cmath>

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
            void correctNodeState(ICalcNode& node, const MaterialPtr& material);
        };
}
#endif /* IDEALPLASTICFLOWCORRECTOR_HPP */
