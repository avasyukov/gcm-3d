#ifndef _GCM_RHEOLOGY_MATRIX__H
#define _GCM_RHEOLOGY_MATRIX__H  1

#include <memory>

#include "libgcm/node/ICalcNode.hpp"
#include "libgcm/util/matrixes.hpp"
#include "libgcm/Exception.hpp"
#include "libgcm/util/Types.hpp"
#include "libgcm/util/Assertion.hpp"
#include "libgcm/rheology/Material.hpp"
#include "libgcm/rheology/setters/ISetter.hpp"
#include "libgcm/rheology/decomposers/IDecomposer.hpp"
#include "libgcm/rheology/correctors/ICorrector.hpp"

using namespace std;

namespace gcm
{

    /**
     * Holds rheology matrix A and provides a way to compute «decomposition» of
     * original matrix to 3 matrices \f$U\f$, \f$\Lambda\f$, \f$U^{-1}\f$
     * that  \f$U^{-2} * \Lambda * U == A\f$.
     */

    class RheologyMatrix
    {
    private:
        /**
         * Material this matrix was created for.
         */
        MaterialPtr material;
        /**
         * Matrix setter to use.
         */
        SetterPtr setter;
        /**
         * Matrix decomposer to use.
         */
        DecomposerPtr decomposer;
        /**
         * Node state corrector to use.
         */
        CorrectorPtr corrector;
        /**
         * Matrices cache.
         */
        struct
        {
            gcm_matrix* a;
            gcm_matrix* u;
            gcm_matrix* l;
            gcm_matrix* u1;
            bool* cached;
        } matrices[3];
        /**
         * Immutable flag.
         */
        bool immutable = true;
        /**
         * Index of current matrix in cache.
         */
        int index = 0;
        /**
         * Current direction number.
         */
        unsigned int direction = 0;
    public:
        /**
         * Constructs new rheology matrix.
         *
         * @param material Material to use for this matrix
         * @param setter Rheology matrix setter
         * @param decomposer Rheology matrix decomposer
         */
        RheologyMatrix(const MaterialPtr& material, const SetterPtr& setter, const DecomposerPtr& decomposer);
        /**
         * Constructs new rheology matrix.
         *
         * @param material Material to use for this matrix
         * @param setter Rheology matrix setter
         * @param decomposer Rheology matrix decomposer
         * @param corrector Node state corrector
         */
        RheologyMatrix(const MaterialPtr& material, const SetterPtr& setter, const DecomposerPtr& decomposer, const CorrectorPtr& corrector);
        /**
         * Disabled copy constructor.
         */
        RheologyMatrix(const RheologyMatrix& that);
        /**
         * Destructor
         */
        ~RheologyMatrix();
        /**
         * Returns max matrix eigenvalue.
         * @return max eigenvalue.
         */
        real getMaxEigenvalue() const;
        /**
         * Returns min matrix eigenvalue.
         * @return min eigenvalue.
         */
        real getMinEigenvalue() const;
        /**
         * Returns rheology matrix A.
         * @return A matrix.
         */
        const gcm_matrix& getA() const;
        /**
         * Returns rheology matrix A component.
         * @return A matrix component.
         */
        real getA(unsigned int i, unsigned int j) const;
        /**
         * Returns \f$\Lambda\f$ matrix (contains eigenvalues).
         * @return \f$\Lambda\f$ matrix.
         */
        const gcm_matrix& getL() const;
        /**
         * Returns \f$\Lambda\f$ matrix component.
         * @return \f$\Lambda\f$ matrix component.
         */
        real getL(unsigned int i, unsigned int j) const;
        /**
         * Returns U matrix (contains eigenvectors).
         * @return U matrix.
         */
        const gcm_matrix& getU() const;
        /**
         * Returns U matrix component.
         * @return U matrix component.
         */
        real getU(unsigned int i, unsigned int j) const;
        /**
         * Returns \f$U^{-1}\f$ matrix (inverse U).
         * @return \f$U^{-1}\f$ matrix.
         */
        const gcm_matrix& getU1() const;
        /**
         * Returns \f$U^{-1}\f$ matrix component.
         * @return \f$U^{-1}\f$ matrix component.
         */
        real getU1(unsigned int i, unsigned int j) const;
        /**
         * Returns material.
         *
         * @return material
         */
        const MaterialPtr& getMaterial() const;      
        /**
         * Computes rheology matrix decomposition (\f$A_x\f$).
         *
         * @param node Node to compute decomposition in.
         */
        void decomposeX(const ICalcNode& node);
        /**
         * Computes rheology matrix decomposition (\f$A_y\f$).
         *
         * @param node Node to compute decomposition in.
         */
        void decomposeY(const ICalcNode& node);
        /**
         * Computes rheology matrix decomposition (\f$A_z\f$).
         *
         * @param node Node to compute decomposition in.
         */
        void decomposeZ(const ICalcNode& node);
        /**
         * Computes rheology matrix decomposition for specified direction.
         *
         * @param node Node to compute decomposition in
         * @param direction Direction to compute decomposition for
         */
        void decompose(const ICalcNode& node, unsigned int direction);
        
        void applyCorrector(ICalcNode& node);
    };

    typedef shared_ptr<RheologyMatrix> RheologyMatrixPtr;

    template<typename...Args>
    RheologyMatrixPtr makeRheologyMatrixPtr(Args...args)
    {
        return make_shared<RheologyMatrix>(args...);
    }

    template<typename SetterType, typename DecomposerType>
    RheologyMatrixPtr  makeRheologyMatrixPtr(MaterialPtr material)
    {
        return makeRheologyMatrixPtr(material, makeSetterPtr<SetterType>(), makeDecomposerPtr<DecomposerType>());
    }
}

#endif
