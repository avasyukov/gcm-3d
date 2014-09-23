#ifndef ICALC_NODE_H_
#define ICALC_NODE_H_

#include "libgcm/rheology/Material.hpp"
#include "libgcm/node/Node.hpp"
#include "libgcm/util/Types.hpp"

#define VALUES_NUMBER 9
using namespace std;
using namespace gcm;

namespace gcm
{

    class Material;

    class ICalcNode : public Node
    {
    public:
        ICalcNode(int num, float x, float y, float z);
        ICalcNode(const ICalcNode& src);

        union
        {
            real values[VALUES_NUMBER];

            struct
            {

                union
                {
                    real velocity[3];

                    struct
                    {
                        /**
                         * Velocity vector x component.
                         */
                        real vx;
                        /**
                         * Velocity vector y component.
                         */
                        real vy;
                        /**
                         * Velocity vector z component.
                         */
                        real vz;
                    };
                };

                union
                {
                    real stress[6];

                    struct
                    {
                        /**
                         * Stress tensor xx component.
                         */
                        real sxx;
                        /**
                         * Stress tensor xy component.
                         */
                        real sxy;
                        /**
                         * Stress tensor xz component.
                         */
                        real sxz;
                        /**
                         * Stress tensor yy component.
                         */
                        real syy;
                        /**
                         * Stress tensor yz component.
                         */
                        real syz;
                        /**
                         * Stress tensor zz component.
                         */
                        real szz;
                    };
                };
            };
        };

        /**
         * Returns density value for node.
         * @return Density value.
         */
        virtual real getRho() const = 0;
        
        virtual void setDestroyed(bool value) = 0;
        virtual bool isDestroyed() const = 0;
        virtual void setDamageMeasure(real value) = 0;
        virtual gcm::real getDamageMeasure() const = 0;
        virtual MaterialPtr getMaterial() const = 0;
        virtual void getMainStressComponents(gcm::real& s1, gcm::real& s2, gcm::real& s3) const = 0;
        virtual void createCrack(int direction) = 0;
        virtual void exciseByCrack() = 0;
    };
}

#endif
