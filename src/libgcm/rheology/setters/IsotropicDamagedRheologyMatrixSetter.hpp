#ifndef ISOTROPICDAMAGEDRHEOLOGYMATRIXSETTER_HPP
#define ISOTROPICDAMAGEDRHEOLOGYMATRIXSETTER_HPP 
#include "libgcm/rheology/setters/ISetter.hpp"

namespace gcm
{
        class IsotropicDamagedRheologyMatrixSetter: public ISetter
        {
            public:
//                /**
//                * Returns damage measure for specified node.
//                * @param node Node to return damage measure for
//                *
//                * @return Node damage measure
//                */
//                real getDamageForNode(const ICalcNode& node) const;
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
                 * @param node Node to set matrix at
                 */
                void setX(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node) override;
                /**
                 * Fills matrix \f$A_y\f$ using specified material parameters.
                 *
                 * @param a Matrix to fill
                 * @param material Material to use
                 * @param node Node to set matrix at
                 */
                void setY(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node) override;
                /**
                 * Fills matrix \f$A_z\f$ using specified material parameters.
                 *
                 * @param a Matrix to fill
                 * @param material Material to use
                 * @param node Node to set matrix at
                 */
                void setZ(gcm_matrix& a, const MaterialPtr& material, const ICalcNode& node) override;
        };
}
#endif /* ISOTROPICDAMAGEDRHEOLOGYMATRIXSETTER_HPP */
