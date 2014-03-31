#ifndef _GCM_ANISOTROPIC_MATRIX_3D_H
#define _GCM_ANISOTROPIC_MATRIX_3D_H  1

#include <assert.h>

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

#include "libgcm/util/matrixes.h"
#include "libgcm/util/ImmutableRheologyMatrix3D.h"
#include "libgcm/materials/IAnisotropicElasticMaterial.h"
#include "libgcm/Exception.h"

using namespace gcm;

namespace gcm
{

    /**
     * Anisotropic rheology matrix implementation. Creates corresponding
     * rheology matrices for case of anisotropic material. Params in this case
     * contain special structure 'AnisotropicNumbers' (at matrixes.h).
     */
     // FIXME
     // This class should be declared as final to allow compiler make different virtual calls optimizations
    class AnisotropicMatrix3D /*final*/: public  ImmutableRheologyMatrix3D
    {
    protected:
        void decompositeIt(gsl_matrix* a, gsl_vector_complex* eval, gsl_matrix_complex* evec);
        void initializeAx(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) override;
        void initializeAy(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) override;
        void initializeAz(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) override;
    private:
        void gcmTogsl(const gcm_matrix &a, gsl_matrix* b);
        void gslTogcm(gsl_matrix* a, gcm_matrix& b);
        void clear(gsl_matrix* a);
        void realChecker(gsl_vector_complex* a, gsl_matrix* l);
        void realChecker(gsl_matrix_complex* a, gsl_matrix* u, int stage);
    public:
        AnisotropicMatrix3D();
    };
}

#endif
