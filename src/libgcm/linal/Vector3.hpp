#ifndef LIBGCM_LINAL_VECTOR3_HPP
#define LIBGCM_LINAL_VECTOR3_HPP

#include "libgcm/linal/Matrix.hpp"
#include "libgcm/linal/Matrix33.hpp"

namespace gcm
{
    namespace linal
    {
        /**
         * Container for vector3 values
         */
        class Vector3Container
        {
            public:
                union
                {
                    real values[3];
                    struct
                    {
                        union
                        {
                            real a1;
                            real x;
                        };
                        union
                        {
                            real a2;
                            real y;
                        };
                        union
                        {
                            real a3;
                            real z;
                        };
                    };
                };
        };

        typedef Matrix<3, 1, Vector3Container> Vector3;
        
        /**
         * Computes vector length.
         *
         * @param v Vector to compute length of.
         *
         * @return Vector length.
         */
        real vectorLength(const Vector3& v);

        /**
         * Normalizes vector.
         *
         * @param v Vector to normalize.
         *
         * @return Normalized vector.
         */
        Vector3 vectorNormalize(const Vector3& v);

        /**
         * Normalizes vector.
         *
         * @param v Vector to normalize.
         *
         * @return Vector length before normalization.
         */
        real vectorNormalizeInplace(Vector3& v);

        /**
         * Computes dot (scalar) product of two vectors.
         *
         * @param v1 First vector.
         * @param v2 Second Vector.
         *
         * @return Dot product.
         */
        real dotProduct(const Vector3& v1, const Vector3& v2);
        
        /**
         * Computes cross (vector) product of two vectors.
         *
         * @param v1 First vector.
         * @param v2 Second vector.
         *
         * @return Cross product.
         */
        Vector3 crossProduct(const Vector3& v1, const Vector3& v2);

        Vector3 operator*(const Matrix33& m, const Vector3& v);
    };
};

#endif
