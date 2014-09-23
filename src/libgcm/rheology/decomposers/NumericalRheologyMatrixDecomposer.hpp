#ifndef ANISOTROPICRHEOLOGYMATRIXDECOMPOSER_HPP
#define ANISOTROPICRHEOLOGYMATRIXDECOMPOSER_HPP 

#include "libgcm/rheology/decomposers/IDecomposer.hpp"

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

namespace gcm
{
    class NumericalRheologyMatrixDecomposer: public IDecomposer
    {
        private:
            mutable gsl_vector_complex* eval;
            mutable gsl_matrix_complex* evec;
            mutable gsl_matrix* _a;
            mutable gsl_matrix* _u;
            mutable gsl_matrix* _u1;
            mutable gsl_permutation* perm;
            mutable gsl_eigen_nonsymmv_workspace* w;
            /**
             * Converts gsl matrix to gcm one.
             *
             * @param a Matrix to convert
             * @param b Matrix to store result
             */
            void gsl2gcm(const gsl_matrix* a, gcm_matrix& b) const;
            /**
             * Converts gcm matrix to gsl one.
             *
             * @param a Matrix to convert
             * @param b Matrix to store result
             */
            void gcm2gsl(const gcm_matrix& a, gsl_matrix* b) const;
            /**
             * Performs matrix decomposition.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             * @param stage Stage to perform decomposition for.
             */
            void decompose(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1, int stage) const;
        public:
            /**
             * Constructor.
             */
            NumericalRheologyMatrixDecomposer();
            /**
             * Deconstructor.
             */
            ~NumericalRheologyMatrixDecomposer();
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
};

#endif /* ANISOTROPICRHEOLOGYMATRIXDECOMPOSER_HPP */
