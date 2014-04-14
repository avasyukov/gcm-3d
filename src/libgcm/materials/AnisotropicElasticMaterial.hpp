/*
 * File:   AnisotropicElasticMaterial.h
 * Author: Alexey Ermakov
 *
 * Created on March 4, 2014, 2:29 PM
 */

#ifndef ANISOTROPICELASTICMATERIAL_H
#define ANISOTROPICELASTICMATERIAL_H

#include "libgcm/materials/IAnisotropicElasticMaterial.hpp"
#include "libgcm/materials/Material.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/util/AnisotropicMatrix3D.hpp"
#include "libgcm/util/AnisotropicMatrix3DAnalytical.hpp"
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

        /**
         * Returns material parameters.
         *
         * @return Material rheology parameters.
         */
        const RheologyParameters& getParameters() const;

        /**
         * Changes material parameters assuming it was rotated
         *
         * @param a1 rotation angle
         * @param a2 rotation angle
         * @param a3 rotation angle
         */
        void rotate(double a1, double a2, double a3);
    };
    
    template<typename MatrixImplementation>
    class AnisotropicElasticMaterialImplementation: public AnisotropicElasticMaterial
    {
    protected:
        MatrixImplementation rheologyMatrix;
    public:
        /**
         * Constructor. Constructs material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Anisotropic rheology parameters
         */
		AnisotropicElasticMaterialImplementation(string name, gcm_real rho, gcm_real crackThreshold, RheologyParameters params):
            AnisotropicElasticMaterial(name, rho, crackThreshold, params)
        {
        }
        /**
         * Returns previously computed rheology matrix.
         *
         * @return Rheology matrix
         */
        RheologyMatrix3D& getRheologyMatrix() override
        {
            return rheologyMatrix;
        }
    };

    typedef AnisotropicElasticMaterialImplementation<AnisotropicMatrix3D> NumericalAnisotropicElasticMaterial;
    typedef AnisotropicElasticMaterialImplementation<AnisotropicMatrix3DAnalytical> AnalyticalAnisotropicElasticMaterial;

}

#endif    /* ANISOTROPICELASTICMATERIAL_H */

