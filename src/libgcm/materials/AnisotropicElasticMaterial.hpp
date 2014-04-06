/*
 * File:   AnisotropicElasticMaterial.h
 * Author: Alexey Ermakov
 *
 * Created on March 4, 2014, 2:29 PM
 */

#ifndef ANISOTROPICELASTICMATERIAL_H
#define    ANISOTROPICELASTICMATERIAL_H

#include "libgcm/materials/IAnisotropicElasticMaterial.hpp"
#include "libgcm/materials/Material.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/util/AnisotropicMatrix3D.hpp"
#include "libgcm/util/Types.hpp"


using namespace std;

namespace gcm
{
    /**
     * Anisotropic elastic material implementation.
     */
    class AnisotropicElasticMaterial : public Material, public IAnisotropicElasticMaterial
    {
    protected:
        /**
         * Rheology parameters.
         */
        RheologyParameters rheologyParameters;
        /**
         * Rheology matrix.
         */
        AnisotropicMatrix3D matrix;
    public:
        /**
         * Constructor. Constructs material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Anisotropic rheology parameters
         */
        AnisotropicElasticMaterial(string name, gcm_real rho, gcm_real crackThreshold, RheologyParameters params);
        ~AnisotropicElasticMaterial();

        /**
         * Returns material parameters.
         *
         * @return Material rheology parameters.
         */
        const RheologyParameters& getParameters() const;

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
        AnisotropicMatrix3D& getRheologyMatrix() override;

        /**
         * Changes material parameters assuming it was rotated
         *
         * @param a1 rotation angle
         * @param a2 rotation angle
         * @param a3 rotation angle
         */
        void rotate(float a1, float a2, float a3);
    };
}

#endif    /* ANISOTROPICELASTICMATERIAL_H */

