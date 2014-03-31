/*
 * File:   ElasticMaterial.h
 * Author: Alexey Ermakov
 *
 * Created on March 4, 2014, 1:07 PM
 */

#ifndef ELASTICMATERIAL_H
#define    ELASTICMATERIAL_H

#include "libgcm/materials/Material.hpp"
#include "libgcm/util/ElasticMatrix3D.hpp"

namespace gcm
{

    /**
     * Isotropic elastic material implementation.
     */
    class IsotropicElasticMaterial : public Material
    {
    protected:
        /**
         * \f$\lambda\f$ Lame parameter.
         */
        gcm_real la;
        /**
         * \f$\mu\f$ Lame parameter.
         */
        gcm_real mu;
        /**
         * Rheology matrix.
         */
        ElasticMatrix3D matrix;
    public:
        /**
         * Constructor. Constructs material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param la \f$\lambda\f$ Lame parameter
         * @param mu \f$\mu\f$ Lame parameter
         */
        IsotropicElasticMaterial(string name, gcm_real rho, gcm_real crackThreshold, gcm_real la, gcm_real mu);
        ~IsotropicElasticMaterial();

        /**
         * Returns \f$\lambda\f$ parameter value.
         * @return \f$\lambda\f$ value
         */
        gcm_real getLambda() const;
        /**
         * Returns \f$\mu\f$ parameter value.
         * @return \f$\mu\f$ value
         */
        gcm_real getMu() const;

//        /**
//         * Recomputes \f$A_x\f$ matrix with respect to node state.
//         *
//         * @param node Mesh node
//         */
//        void prepareRheologyMatrixX(const CalcNode& node) override;
//        /**
//         * Recomputes \f$A_y\f$ matrix with respect to node state.
//         *
//         * @param node Mesh node
//         */
//        void prepareRheologyMatrixY(const CalcNode& node) override;
//        /**
//         * Recomputes \f$A_z\f$ matrix with respect to node state.
//         *
//         * @param node Mesh node
//         */
//        void prepareRheologyMatrixZ(const CalcNode& node) override;
        /**
         * Returns previously computed rheology matrix.
         *
         * @return Rheology matrix
         */
        ElasticMatrix3D& getRheologyMatrix();
    };
}

#endif    /* ELASTICMATERIAL_H */

