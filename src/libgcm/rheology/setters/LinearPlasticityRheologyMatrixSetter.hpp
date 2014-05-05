#ifndef LINEARPLASTICITYRHEOLOGYMATRIXSETTER_HPP
#define LINEARPLASTICITYRHEOLOGYMATRIXSETTER_HPP

#include "libgcm/rheology/setters/ISetter.hpp"

namespace gcm
{
        class LinearPlasticityRheologyMatrixSetter: public ISetter
        {
            protected:
                /**
                 * Yield limit.
                 */
                float yieldLimit;
                /**
                 * TODO document
                 */
                float normE;
                /**
                 * Extracts plasticity parameters
                 *
                 * @param material Material
                 * @param yieldLimit Yield limit
                 * @param normE TODO document
                 */
                void getPlasticityProps(MaterialPtr material, float& yieldLimit, float& normE);
            public:
                /**
                 * Returns number of matrix states after decompositions. This method is supposed to be used to
                 * preallocate memory for matrices cache.
                 *
                 * @return Number of possible states (for one space direction) or 0 if unknown.
                 */
                unsigned int getNumberOfStates() const;
                /**
                 * Returns number of state for specified node.
                 *
                 * @param node Node to return state for
                 *
                 * @return Node state
                 */
                unsigned int getStateForNode(const ICalcNode& node) const;
                /**
                 * Fills matrix \f$A_x\f$ using specified material parameters.
                 *
                 * @param a Matrix to fill
                 * @param material Material to use
                 */
                void setX(gcm_matrix& a, const MaterialPtr& material) override;
                /**
                 * Fills matrix \f$A_y\f$ using specified material parameters.
                 *
                 * @param a Matrix to fill
                 * @param material Material to use
                 */
                void setY(gcm_matrix& a, const MaterialPtr& material) override;
                /**
                 * Fills matrix \f$A_z\f$ using specified material parameters.
                 *
                 * @param a Matrix to fill
                 * @param material Material to use
                 */
                void setZ(gcm_matrix& a, const MaterialPtr& material) override;
        };
}
#endif /* LINEARPLASTICITYRHEOLOGYMATRIXSETTER_HPP */
