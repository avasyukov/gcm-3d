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
        class Matrix33: public Matrix<3, 3, Matrix33Container>
        {
            public:
                /**
                 * Default constructor.
                 */
                Matrix33();

                /**
                 * Copy constructor
                 *
                 * @param m Matrix to construct from.
                 */
                Matrix33(const Matrix33& m);

                /**
                 * Assignment operator.
                 *
                 * @param m Matrix to assign values from.
                 *
                 * @return Reference to modified matrix instance.
                 */
                Matrix33& operator=(const Matrix33& m);
                /**
                 * Constructor that initializes matrix with specified values.
                 *
                 * @param values Values to initialize matrix with.
                 */
                Matrix33(std::initializer_list<real> values);
                /**
                 * Calculates matrix determinant.
                 *
                 * @return Determinant value.
                 */
                real determinant() const;
        };
    };
};
#endif 
