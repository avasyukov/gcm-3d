#ifndef ANALYTICALRHEOLOGYMATRIXDECOMPOSER_HPP
#define ANALYTICALRHEOLOGYMATRIXDECOMPOSER_HPP

#include "libgcm/rheology/decomposers/IDecomposer.hpp"

#include "libgcm/util/matrixes.hpp"
#include "libgcm/util/ThirdDegreePolynomial.hpp"
#include "libgcm/Exception.hpp"
#include <assert.h>

namespace gcm
{
    class AnalyticalRheologyMatrixDecomposer : public IDecomposer
    {
		private:
			void findNonZeroSolution(double **M, double *x) const;
			void findNonZeroSolution(double **M, double *x, double *y) const;
			void findEigenVec(double *eigenVec,
						double l, const gcm_matrix &A, int stage) const;
			void findEigenVec(double *eigenVec1, double *eigenVec2,
						double l, const gcm_matrix &A, int stage) const;
        public:
            /**
             * Constructor.
             */
            AnalyticalRheologyMatrixDecomposer();
            /**
             * Deconstructor.
             */
            ~AnalyticalRheologyMatrixDecomposer();
            /**
             * Computes decomposition for matrix in X direction.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             */
            void decomposeX(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const;
            /**
             * Computes decomposition for matrix in Y direction.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             */
            void decomposeY(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const;
            /**
             * Computes decomposition for matrix in Z direction.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             */
            void decomposeZ(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const;
    };
}

#endif /* ANALYTICALRHEOLOGYMATRIXDECOMPOSER_HPP */