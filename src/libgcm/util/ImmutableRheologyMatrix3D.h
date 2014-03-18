#ifndef IMMUTABLE_RHEOLOGY_MATRIX_3D_H
#define IMMUTABLE_RHEOLOGY_MATRIX_3D_H

#include "util/RheologyMatrix3D.h"

namespace gcm {
    class ImmutableRheologyMatrix3D: public RheologyMatrix3D {
    private:
        bool matrixCreated[3];
        int currentMatrix;

        gcm_matrix A[3];
        gcm_matrix L[3];
        gcm_matrix U[3];
        gcm_matrix U1[3];

    protected:

        virtual void initializeAx(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) = 0;
        virtual void initializeAy(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) = 0;
        virtual void initializeAz(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) = 0;
    public:
        /**
         * Constructor.
         */
         ImmutableRheologyMatrix3D();
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

        void createAx(const ICalcNode& node) override;
        void createAy(const ICalcNode& node) override;
        void createAz(const ICalcNode& node) override;
        void refresh() override;
    };
}

#endif /* IMMUTABLE_RHEOLOGY_MATRIX_3D_H */