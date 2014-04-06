#ifndef _GCM_ELASTIC_MATRIX_3D_H
#define _GCM_ELASTIC_MATRIX_3D_H  1

#include <assert.h>

#include "libgcm/util/matrixes.hpp"
#include "libgcm/util/ImmutableRheologyMatrix3D.hpp"
#include "libgcm/Exception.hpp"

using namespace gcm;
using namespace std;

namespace gcm
{

    /**
     * @brief Elastic rheology matrix implementation.
     * @details Creates corresponding rheology matrices for case
     *          of elastic  material. Params in this case contain
     *          lambda, mu and rho.
     */
    class ElasticMatrix3D final : public ImmutableRheologyMatrix3D
    {
    protected:
        // void clear();
        void initializeAx(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) override;
        void initializeAy(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) override;
        void initializeAz(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1) override;
    // public:
    //     void createAx(const ICalcNode& node) override;
    //     void createAy(const ICalcNode& node) override;
    //     void createAz(const ICalcNode& node) override;
//        /**
//         * Prepares matrices using specified values. Creates
//         * rheology matrices (\f$A_x\f$, \f$A_y\f$, \f$A_z\f$) depending
//         * on stage.
//         *
//         * @param lambda Lame \f$\lambda\f$ parameter
//         * @param mu Lame \f$\mu\f$ parameter
//         * @param rho Material density
//         * @param stage Calculation stage
//         */
//        void prepare(gcm_real lamba, gcm_real mu, gcm_real rho, uint stage);
    };
}

#endif
