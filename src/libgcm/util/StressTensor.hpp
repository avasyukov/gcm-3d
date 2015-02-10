#ifndef GCM_UTIL_TENSORT
#define GCM_UTIL_TENSORT

#include "libgcm/util/Types.hpp"
#include "libgcm/linal/Matrix33.hpp"
#include "libgcm/linal/Vector3.hpp"
#include "libgcm/rheology/Material.hpp"

namespace gcm
{
    /**
     * Stress tensor umplementation.
     */
    class StressTensor
    {
        public:
            union
            {
                real values[6];
                struct
                {
                    real xx;
                    union
                    {
                        real xy;
                        real yx;
                    };
                    union
                    {
                        real xz;
                        real zx;
                    };
                    real yy;
                    union
                    {
                        real yz;
                        real zy;
                    };
                    real zz;
                };
            };

            /**
             * Default constructor.
             */
            StressTensor();
            /**
             * Constructor that fills StressTensor with passed values.
             *
             * @param xx XX tensor component.
             * @param xy XY tensor component.
             * @param xz XZ tensor component.
             * @param yy YY tensor component.
             * @param yz YZ tensor component.
             * @param zz ZZ tensor component.
             */
            StressTensor(real xx, real xy, real xz, real yy, real yz, real zz);

            /**
             * Computes tensor trace.
             *
             * @return Tensor trace.
             */
            real trace() const;
            /**
             * Transforms tensor components according to passed transformation matrix.
             *
             * @param s Transformation matrix.
             */
            void transform(const linal::Matrix33& s);

            /**
             * Sets tensor components to values corresponding P-wave propogation along specific direction.
             *
             * @param material Material to be used to set tensor components.
             * @param direction P-wave propogation direction.
             */
            void setToIsotropicPWave(const MaterialPtr& material, const linal::Vector3& direction);
    };
}

#endif
