#ifndef LIBGCM_LINAL_MATRIX33_HPP
#define LIBGCM_LINAL_MATRIX33_HPP

#include "libgcm/linal/Matrix.hpp"

namespace gcm
{
    namespace linal
    {
        /**
         * Specialized value container for 3x3 matrix.
         */
        class Matrix33Container
        {
            public:
                union
                {
                    real values[9];
                    struct
                    {
                        union
                        {
                            real xx;
                            real a11;
                        };
                        union
                        {
                            real xy;
                            real a12;
                        };
                        union
                        {
                            real xz;
                            real a13;
                        };
                        union
                        {
                            real yx;
                            real a21;
                        };
                        union
                        {
                            real yy;
                            real a22;
                        };
                        union
                        {
                            real yz;
                            real a23;
                        };
                        union
                        {
                            real zx;
                            real a31;
                        };
                        union
                        {
                            real zy;
                            real a32;
                        };
                        union
                        {
                            real zz;
                            real a33;
                        };
                    };
                };
        };
        /**
         * Specialized 3x3 matrix implementation.
         */
        typedef Matrix<3, 3, Matrix33Container> Matrix33;

        /**
         * Calculates matrix determinant.
         *
         * @return Determinant value.
         */
        real determinant(const Matrix33& m);

        /**
         * Rotate 3*3 matrix by angles around the axis.
         *
         * @param m Reference to original matrix
         * @param a1 An angle to rotate around X axis
         * @param a2 An angle to rotate around Y axis
         * @param a3 An angle to rotate around Z axis
         * @return Rotated matrix.
         */
        Matrix33 rotate(const Matrix33& m, real a1, real a2, real a3);
    };
};
#endif 
