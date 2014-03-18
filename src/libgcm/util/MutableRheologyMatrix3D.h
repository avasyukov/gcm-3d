#ifndef MUTABLE_RHEOLOGY_MATRIX_3D_H
#define MUTABLE_RHEOLOGY_MATRIX_3D_H

#include "util/RheologyMatrix3D.h"

namespace gcm {
    class MutableRheologyMatrix3D: public RheologyMatrix3D {
    protected:

        gcm_matrix A;
        gcm_matrix L;
        gcm_matrix U;
        gcm_matrix U1;

    public:
        /**
         * Constructor.
         */
         MutableRheologyMatrix3D();
        /**
         * Returns max matrix eigenvalue.
         * @return max eigenvalue.
         */
        gcm_real getMaxEigenvalue() const override;
        /**
         * Returns min matrix eigenvalue.
         * @return min eigenvalue.
         */
        gcm_real getMinEigenvalue() const override;
        /**
         * Returns rheology matrix A.
         * @return A matrix.
         */
        const gcm_matrix& getA() const override;
        /**
         * Returns \f$\Lambda\f$ matrix (contains eigenvalues).
         * @return \f$\Lambda\f$ matrix.
         */
        const gcm_matrix& getL() const override;
        /**
         * Returns U matrix (contains eigenvectors).
         * @return U matrix.
         */
        const gcm_matrix& getU() const override;
        /**
         * Returns \f$U^{-1}\f$ matrix (inverse U).
         * @return \f$U^{-1}\f$ matrix.
         */
        const gcm_matrix& getU1() const override;
    };
}

#endif /* MUTABLE_RHEOLOGY_MATRIX_3D_H */