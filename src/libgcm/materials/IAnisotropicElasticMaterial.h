/*
 * File:   IAnisotropicElasticMaterial.h
 * Author: Alexey Ermakov
 *
 * Created on March 4, 2014, 9:43 PM
 */

#ifndef IANISOTROPICELASTICMATERIAL_H
#define    IANISOTROPICELASTICMATERIAL_H

#define ANISOTROPIC_ELASTIC_MATERIALS_PARAMETERS_NUM 21

namespace gcm
{
    /**
     * Interface for anisotropic elastic material type. Supposed to be used only
     * to solve circle class dependencies.
     */
    class IAnisotropicElasticMaterial
    {
    public:

        /**
         * Type to hold anisotropic elastic material parameters.
         */
        typedef struct
        {

            union
            {
                /**
                 * All parameters in one array.
                 */
                float values[ANISOTROPIC_ELASTIC_MATERIALS_PARAMETERS_NUM];
                struct
                {
                    float c11;
                    union { float c12; float c21; };
                    union { float c13; float c31; };
                    union { float c14; float c41; };
                    union { float c15; float c51; };
                    union { float c16; float c61; };
                    float c22;
                    union { float c23; float c32; };
                    union { float c24; float c42; };
                    union { float c25; float c52; };
                    union { float c26; float c62; };
                    float c33;
                    union { float c34; float c43; };
                    union { float c35; float c53; };
                    union { float c36; float c63; };
                    float c44;
                    union { float c45; float c54; };
                    union { float c46; float c64; };
                    float c55;
                    union { float c56; float c65; };
                    float c66;

                };
            };
        } RheologyParameters;
        /**
         * Returns material parameters.
         *
         * @return Material rheology parameters.
         */
        virtual const RheologyParameters& getParameters() const = 0;
    };
}

#endif    /* IANISOTROPICELASTICMATERIAL_H */

