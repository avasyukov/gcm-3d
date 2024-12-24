#ifndef PiezoCorrector_HPP
#define PiezoCorrector_HPP
#include "libgcm/rheology/correctors/ICorrector.hpp"
#include "libgcm/mesh/Mesh.hpp"
#include <cmath>

#include <iostream>

namespace gcm
{
        class PulseForm;

        class PiezoCorrector: public ICorrector
        {
                real fx, fy, fz;
                PulseForm* form;
            public:
                PiezoCorrector(real fx, real fy, real fz, real start, real duration);
                /**
                * Perform correction for the node
                *
                * @param node Node to perform correction on
                */
                void correctNodeState(ICalcNode& node, Mesh& mesh, const MaterialPtr& material, float time_step);
        };
}
#endif /* MaxwellViscosityCorrector_HPP */
