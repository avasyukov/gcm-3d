#ifndef _GCM_MATERIAL_H
#define _GCM_MATERIAL_H 1

#include <string>

#include "util/Types.h"
//#include "util/RheologyMatrix3D.h"

using namespace std;

namespace gcm {
    // forward declaration
    class CalcNode;
    class RheologyMatrix3D;
    
    /**
     * Base class to inherit from to implement different material types.
     */
    class Material {
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
    public:
        /**
         * Constructor. Constructs material using specified parameters.
         * 
         * @param name Material name
         * @param rho Material density
         * @param crackThreshold Material crack threshold
         */
        Material(string name, gcm_real rho, gcm_real crackThreshold);
        /**
         * Destructor.
         */
        virtual ~Material() = 0;

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
        
//        /**
//         * Recomputes \f$A_x\f$ matrix with respect to node state.
//         * 
//         * @param node Mesh node
//         */
//        virtual void prepareRheologyMatrixX(const CalcNode& node) = 0;
//        /**
//         * Recomputes \f$A_y\f$ matrix with respect to node state.
//         * 
//         * @param node Mesh node
//         */
//        virtual void prepareRheologyMatrixY(const CalcNode& node) = 0;
//        /**
//         * Recomputes \f$A_z\f$ matrix with respect to node state.
//         * 
//         * @param node Mesh node
//         */
//        virtual void prepareRheologyMatrixZ(const CalcNode& node) = 0;
        
        /**
         * Returns rheology matrix.
         * 
         * @return Rheology matrix
         */
        virtual RheologyMatrix3D& getRheologyMatrix() = 0;
    };
}

#endif