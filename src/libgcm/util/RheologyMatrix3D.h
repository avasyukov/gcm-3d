#ifndef _GCM_RHEOLOGY_MATRIX_3D_H
#define _GCM_RHEOLOGY_MATRIX_3D_H  1

#include <initializer_list>
#include <assert.h>

#include "node/ICalcNode.h"
#include "util/matrixes.h"
#include "Exception.h"
#include "Types.h"

using namespace std;

namespace gcm
{

    /**
     * Base class to inherit rheology matrices implementations from.Holds
     * rheology matrix A and provides a way to compute «decomposition» of
     * original matrix to 3 matrices \f$U\f$, \f$\Lambda\f$, \f$U^{-1}\f$
     * that  \f$U^{-2} * \Lambda * U == A\f$.
     */

    class RheologyMatrix3D
    {
    protected:
        gcm_matrix A;
        gcm_matrix L;
        gcm_matrix U;
        gcm_matrix U1;
    public:
        /**
         * Destructor
         */
        virtual ~RheologyMatrix3D() = 0;
        /**
         * Returns max matrix eigenvalue.
         * @return max eigenvalue.
         */
        gcm_real getMaxEigenvalue() const;
        /**
         * Returns min matrix eigenvalue.
         * @return min eigenvalue.
         */
        gcm_real getMinEigenvalue() const;
        /**
         * Returns rheology matrix A.
         * @return A matrix.
         */
        gcm_matrix& getA();
        /**
         * Returns rheology matrix A component.
         * @return A matrix component.
         */
        gcm_real getA(unsigned int i, unsigned int j) const;
        /**
         * Returns \f$\Lambda\f$ matrix (contains eigenvalues).
         * @return \f$\Lambda\f$ matrix.
         */
        gcm_matrix& getL();
        /**
         * Returns \f$\Lambda\f$ matrix component.
         * @return \f$\Lambda\f$ matrix component.
         */
        gcm_real getL(unsigned int i, unsigned int j) const;
        /**
         * Returns U matrix (contains eigenvectors).
         * @return U matrix.
         */
        gcm_matrix& getU();
        /**
         * Returns U matrix component.
         * @return U matrix component.
         */
        gcm_real getU(unsigned int i, unsigned int j) const;
        /**
         * Returns \f$U^{-1}\f$ matrix (inverse U).
         * @return \f$U^{-1}\f$ matrix.
         */
        gcm_matrix& getU1();
        /**
         * Returns \f$U^{-1}\f$ matrix component.
         * @return \f$U^{-1}\f$ matrix component.
         */
        gcm_real getU1(unsigned int i, unsigned int j) const;



        virtual void createAx(const ICalcNode& node) = 0;
        virtual void createAy(const ICalcNode& node) = 0;
        virtual void createAz(const ICalcNode& node) = 0;
    };
}

#endif
