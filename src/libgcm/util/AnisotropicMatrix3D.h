#ifndef _GCM_ANISOTROPIC_MATRIX_3D_H
#define _GCM_ANISOTROPIC_MATRIX_3D_H  1

#include <assert.h>

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h> 
#include <gsl/gsl_blas.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

#include "util/matrixes.h"
#include "util/RheologyMatrix3D.h"
#include "materials/IAnisotropicElasticMaterial.h"
#include "Exception.h"

using namespace gcm;

namespace gcm
{

    /**
     * Anisotropic rheology matrix implementation. Creates corresponding
     * rheology matrices for case of anisotropic material. Params in this case
     * contain special structure 'AnisotropicNumbers' (at matrixes.h).
     */
    class AnisotropicMatrix3D : public RheologyMatrix3D
    {
    protected:
        void clear();
        void decompositeIt(gsl_matrix* a, gsl_vector_complex* eval, gsl_matrix_complex* evec);
    private:
        void gcmTogsl(const gcm_matrix &a, gsl_matrix* b);
        void gslTogcm(gsl_matrix* a, gcm_matrix& b);
        void clear(gsl_matrix* a);
        void realChecker(gsl_vector_complex* a, gsl_matrix* l);
        void realChecker(gsl_matrix_complex* a, gsl_matrix* u, int stage);
    public:
        void createAx(const ICalcNode& node) override;
        void createAy(const ICalcNode& node) override;
        void createAz(const ICalcNode& node) override;
    };
}

#endif
