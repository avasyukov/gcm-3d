#ifndef PRANDTLREISSPLASTICITYRHEOLOGYMATRIXSETTER_HPP
#define PRANDTLREISSPLASTICITYRHEOLOGYMATRIXSETTER_HPP

#include "libgcm/rheology/setters/ISetter.hpp"
#include "libgcm/Math.hpp"

namespace gcm
{
        class PrandtlRaussPlasticityRheologyMatrixSetter: public ISetter
        {
            protected:
				float q[3][3][3][3];
				void computeQ(const MaterialPtr& material, const ICalcNode& node);
            
                /**
                 * Extracts plasticity parameters
                 *
                 * @param material Material
                 * @param yieldLimit Yield limit
                 * @param normE TODO document
                 */
                void getPlasticityProps(MaterialPtr material, float& yieldStrength);
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
#endif /* PRANDLREISSPLASTICITYRHEOLOGYMATRIXSETTER_HPP */
