#ifndef _GCM_MATERIAL_H
#define _GCM_MATERIAL_H 1

#include <string>
#include <memory>
#include <map>

#include "libgcm/util/Types.hpp"

using namespace std;

namespace gcm {
    /**
     * Holds information about material properties.
     */
    class Material {
    public:
        /**
         * Number of rheology parameters.
         */
        static const int RHEOLOGY_PROPERTIES_NUMBER = 21;
        /**
         * Type to hold anisotropic material parameters.
         */
        class RheologyProperties
        {
            public:
                /**
                 * Default constructor.
                 */
                RheologyProperties();
                /**
                 * Constructor. Fills rheology parameters using isotropic parameters.
                 *
                 * @param la \f$\lambda\f$ Lame parameter
                 * @param mu \f$\mu\f$ Lame parameter
                 */
                RheologyProperties(gcm_real la, gcm_real mu);
                /**
                 * Constructor. Fills rheology parameters using anisotropic parameters.
                 *
                 * @param params Anisotropic rheology parameters
                 */
                RheologyProperties(const gcm_real (&params)[RHEOLOGY_PROPERTIES_NUMBER]);

                union
                {
                    /**
                    * All parameters in one array.
                    */
                    gcm_real values[RHEOLOGY_PROPERTIES_NUMBER];
                    struct
                    {
                        gcm_real c11;
                        union { gcm_real c12; gcm_real c21; };
                        union { gcm_real c13; gcm_real c31; };
                        union { gcm_real c14; gcm_real c41; };
                        union { gcm_real c15; gcm_real c51; };
                        union { gcm_real c16; gcm_real c61; };
                        gcm_real c22;
                        union { gcm_real c23; gcm_real c32; };
                        union { gcm_real c24; gcm_real c42; };
                        union { gcm_real c25; gcm_real c52; };
                        union { gcm_real c26; gcm_real c62; };
                        gcm_real c33;
                        union { gcm_real c34; gcm_real c43; };
                        union { gcm_real c35; gcm_real c53; };
                        union { gcm_real c36; gcm_real c63; };
                        gcm_real c44;
                        union { gcm_real c45; gcm_real c54; };
                        union { gcm_real c46; gcm_real c64; };
                        gcm_real c55;
                        union { gcm_real c56; gcm_real c65; };
                        gcm_real c66;
                    };
                };
                /**
                 * Returns \f$\mu\f$ parameter value in assumption that material is isotropic.
                 *
                 * @return \f$\mu\f$ parameter value.
                 */
                gcm_real getMu() const;
                /**
                 * Returns \f$\lambda\f$ parameter value in assumption that material is isotropic.
                 *
                 * @return \f$\lambda\f$ parameter value.
                 */
                gcm_real getLa() const;
                /**
                * Changes rheology parameters assuming it was rotated
                *
                * @param a1 rotation angle
                * @param a2 rotation angle
                * @param a3 rotation angle
                */
                void rotate(double a1, double a2, double a3);
        };
        typedef map<string, map<string, float>> PlasticityProperties;
    protected:
        /**
         * Material name.
         */
        string name;
        /**
         * Material density.
         */
        gcm_real rho;
        /**
         * Material crack threshold.
         */
        gcm_real crackThreshold;
        /**
         * Rheology properties.
         */
        RheologyProperties rheologyProps;
        /**
         * Plasticity properties.
         */
        PlasticityProperties plasticityProps;
        /**
         * Isotropic flag.
         */
        bool isotropic = false;
    public:
        /**
         * Constructor. Constructs isotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param la \f$\lambda\f$ Lame parameter
         * @param mu \f$\mu\f$ Lame parameter
         */
        Material(string name, gcm_real rho, gcm_real crackThreshold, gcm_real la, gcm_real mu);
        /**
         * Constructor. Constructs isotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param la \f$\lambda\f$ Lame parameter
         * @param mu \f$\mu\f$ Lame parameter
         * @param plasticityProps Plasticity properties
         */
        Material(string name, gcm_real rho, gcm_real crackThreshold, gcm_real la, gcm_real mu, PlasticityProperties plasticityProps);
        /**
         * Constructor. Constructs anisotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Material rheology parameters
         */
        Material(string name, gcm_real rho, gcm_real crackThreshold, RheologyProperties rheologyProps);
        /**
         * Constructor. Constructs anisotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Material rheology parameters
         * @param plasticityProps Plasticity properties
         */
        Material(string name, gcm_real rho, gcm_real crackThreshold, RheologyProperties rheologyProps, PlasticityProperties plasticityProps);
        /**
         * Constructor. Constructs anisotropic material by rotation of passed material.
         *
         * @param source Source material
         * @param name Material name
         * @param a1 a1 rotation angle
         * @param a2 a2 rotation angle
         * @param a3 a3 rotation angle
         */
        Material(const shared_ptr<Material>& source, string name, double a1, double a2, double a3);
        /**
         * Returns material name.
         *
         * @return Material name.
         */
        const string& getName() const;
        /**
         * Returns material density.
         *
         * @return Density value.
         */
        gcm_real getRho() const;
        /**
         * Returns material crack threshold.
         *
         * @return Crack threshold value.
         */
        gcm_real getCrackThreshold() const;
        /**
         * Returns isotropic flag value.
         *
         * @return true if material is isotropic and false otherwise.
         */
        bool isIsotropic() const;
        /**
         * Returns \f$\mu\f$ parameter value.
         *
         * @return \f$\mu\f$ parameter value.
         */
        gcm_real getMu() const;
        /**
         * Shortcut to get \f$\lambda\f$ parameter value.
         *
         * @return \f$\lambda\f$ parameter value.
         */
        gcm_real getLa() const;

        /**
         * Returns material rheology properties.
         *
         * @return Rheology properties.
         */
        const RheologyProperties& getRheologyProperties() const;
        /**
         * Returns material plasticity properties.
         *
         * @return Plasticity properties.
         */
        const PlasticityProperties& getPlasticityProperties() const;
    };

    typedef shared_ptr<Material> MaterialPtr;
    
    template<typename...Args>
    MaterialPtr makeMaterialPtr(Args...args)
    {
        return make_shared<Material>(args...);
    }

}

#endif
