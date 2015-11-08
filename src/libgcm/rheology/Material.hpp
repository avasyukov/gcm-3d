#ifndef _GCM_MATERIAL_H
#define _GCM_MATERIAL_H 1

#include <string>
#include <memory>
#include <map>

#include "libgcm/util/Types.hpp"


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
                RheologyProperties(real la, real mu);
                /**
                 * Constructor. Fills rheology parameters using anisotropic parameters.
                 *
                 * @param params Anisotropic rheology parameters
                 */
                RheologyProperties(const real (&params)[RHEOLOGY_PROPERTIES_NUMBER]);

                union
                {
                    /**
                    * All parameters in one array.
                    */
                    real values[RHEOLOGY_PROPERTIES_NUMBER];
                    struct
                    {
                        real c11;
                        union { real c12; real c21; };
                        union { real c13; real c31; };
                        union { real c14; real c41; };
                        union { real c15; real c51; };
                        union { real c16; real c61; };
                        real c22;
                        union { real c23; real c32; };
                        union { real c24; real c42; };
                        union { real c25; real c52; };
                        union { real c26; real c62; };
                        real c33;
                        union { real c34; real c43; };
                        union { real c35; real c53; };
                        union { real c36; real c63; };
                        real c44;
                        union { real c45; real c54; };
                        union { real c46; real c64; };
                        real c55;
                        union { real c56; real c65; };
                        real c66;
                    };
                };
                /**
                 * Returns \f$\mu\f$ parameter value in assumption that material is isotropic.
                 *
                 * @return \f$\mu\f$ parameter value.
                 */
                real getMu() const;
                /**
                 * Returns \f$\lambda\f$ parameter value in assumption that material is isotropic.
                 *
                 * @return \f$\lambda\f$ parameter value.
                 */
                real getLa() const;
                /**
                * Changes rheology parameters assuming it was rotated
                *
                * @param a1 rotation angle
                * @param a2 rotation angle
                * @param a3 rotation angle
                */
                void rotate(double a1, double a2, double a3);
        };
		// WA: we use 'float' and 'double' to prevent identical constructors
		// signature for initialization with PlasticityProperties only or with FailureProperties only.
        typedef std::map<std::string, std::map<std::string, float>> PlasticityProperties;
        typedef std::map<std::string, std::map<std::string, double>> FailureProperties;
    protected:
        /**
         * Material name.
         */
        std::string name;
        /**
         * Material density.
         */
        real rho;
        /**
         * Rheology properties.
         */
        RheologyProperties rheologyProps;
        /**
         * Plasticity properties.
         */
        PlasticityProperties plasticityProps;
        /**
         * Failure properties.
         */
        FailureProperties failureProps;
        /**
         * Isotropic flag.
         */
        bool isotropic = false;
        /**
         * Angles of rotation of rheology properties around X, Y, Z axis
         */
        vector3r angles;
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
        Material(std::string name, real rho, real la, real mu);
        /**
         * Constructor. Constructs isotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param la \f$\lambda\f$ Lame parameter
         * @param mu \f$\mu\f$ Lame parameter
         */
        Material(std::string name, real rho, real la, real mu, FailureProperties failureProps);
        /**
         * Constructor. Constructs isotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param la \f$\lambda\f$ Lame parameter
         * @param mu \f$\mu\f$ Lame parameter
         */
        Material(std::string name, real rho, real la, real mu, PlasticityProperties plasticityProps);
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
        Material(std::string name, real rho, real la, real mu, PlasticityProperties plasticityProps, FailureProperties failureProps);
        /**
         * Constructor. Constructs anisotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Material rheology parameters
         */
        Material(std::string name, real rho, RheologyProperties rheologyProps);
        /**
         * Constructor. Constructs anisotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Material rheology parameters
         */
        Material(std::string name, real rho, RheologyProperties rheologyProps, FailureProperties failureProps);
        /**
         * Constructor. Constructs anisotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Material rheology parameters
         */
        Material(std::string name, real rho, RheologyProperties rheologyProps, PlasticityProperties plasticityProps);
        /**
         * Constructor. Constructs anisotropic material using specified parameters.
         *
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         * @param params Material rheology parameters
         * @param plasticityProps Plasticity properties
         */
        Material(std::string name, real rho, RheologyProperties rheologyProps, PlasticityProperties plasticityProps, FailureProperties failureProps);
        /**
         * Constructor. Constructs anisotropic material by rotation of passed material.
         *
         * @param source Source material
         * @param name Material name
         * @param a1 a1 rotation angle
         * @param a2 a2 rotation angle
         * @param a3 a3 rotation angle
         */
        Material(const std::shared_ptr<Material>& source, std::string name, double a1, double a2, double a3);
        /**
         * Returns material name.
         *
         * @return Material name.
         */
        const std::string& getName() const;
        /**
         * Returns material density.
         *
         * @return Density value.
         */
        real getRho() const;
        /**
         * Returns failure thresholds
         * @return failureProps.
         */
	const FailureProperties& getFailureProperties() const;
        /**
         * Returns material crack threshold.
         *
         * @return Crack threshold value.
         */
//        real getCrackThreshold() const;
        /**
         * Returns Hashin failure thresholds
         */
//	int getHashinDir() {return Hdir;};
//	real getHashinXc() {return HXc;};
//        real getHashinXt() {return HXt;};
//        real getHashinYc() {return HYc;};
//        real getHashinYt() {return HYt;};
//        real getHashinSt() {return HSt;};
//        real getHashinS() {return HS;};
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
        real getMu() const;
        /**
         * Shortcut to get \f$\lambda\f$ parameter value.
         *
         * @return \f$\lambda\f$ parameter value.
         */
        real getLa() const;

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
        /**
         * Returns angles of rotation of rheology properties
         *
         * @return Vector of three angles
         */
        const vector3r& getAngles() const;
    };

    typedef std::shared_ptr<Material> MaterialPtr;
    
    template<typename...Args>
    MaterialPtr makeMaterialPtr(Args...args)
    {
        return std::make_shared<Material>(args...);
    }

}

#endif
