#ifndef IDECOMPOSER_HPP
#define IDECOMPOSER_HPP 

#include "libgcm/util/matrixes.hpp"

#include <memory>

namespace gcm
{
    /**
     * Interface to implement for rheology matrix decomposer. Decomposer is supposed to be used
     * for matrix decomposition, i.e. to fill matricex \f$U\f$, \f$\Lambda\f$ and \f$U^{-1}\f$.
     */
    class IDecomposer
    {
        public:
            /**
             * Computes decomposition for matrix in X direction.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             */
            virtual void decomposeX(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const = 0;
            /**
             * Computes decomposition for matrix in Y direction.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             */
            virtual void decomposeY(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const = 0;
            /**
             * Computes decomposition for matrix in Z direction.
             *
             * @param a Matrix to decompose.
             * @param u Matrix to store \f$U\f$
             * @param l Matrix to store \f$\Lambda\f$
             * @param u1 Matrix to store \f$U^{-1}\f$
             */
            virtual void decomposeZ(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const = 0;
    };

    typedef std::shared_ptr<IDecomposer> DecomposerPtr;
    
    template<typename T, typename...Args>
    std::shared_ptr<T> makeDecomposerPtr(Args...args)
    {
        return std::make_shared<T>(args...);
    }
};

#endif /* IDECOMPOSER_HPP */
