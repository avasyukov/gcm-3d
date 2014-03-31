#ifndef ICALC_NODE_H_
#define ICALC_NODE_H_

//#include "libgcm/materials/Material.hpp"
#include "libgcm/node/Node.hpp"
#include "libgcm/util/Types.hpp"

#define GCM_VALUES_SIZE 9
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
            gcm_real values[GCM_VALUES_SIZE];

            struct
            {

                union
                {
                    gcm_real velocity[3];

                    struct
                    {
                        /**
                         * Velocity vector x component.
                         */
                        gcm_real vx;
                        /**
                         * Velocity vector y component.
                         */
                        gcm_real vy;
                        /**
                         * Velocity vector z component.
                         */
                        gcm_real vz;
                    };
                };

                union
                {
                    gcm_real stress[6];

                    struct
                    {
                        /**
                         * Stress tensor xx component.
                         */
                        gcm_real sxx;
                        /**
                         * Stress tensor xy component.
                         */
                        gcm_real sxy;
                        /**
                         * Stress tensor xz component.
                         */
                        gcm_real sxz;
                        /**
                         * Stress tensor yy component.
                         */
                        gcm_real syy;
                        /**
                         * Stress tensor yz component.
                         */
                        gcm_real syz;
                        /**
                         * Stress tensor zz component.
                         */
                        gcm_real szz;
                    };
                };
            };
        };

        /**
         * Returns density value for node.
         * @return Density value.
         */
        virtual gcm_real getRho() const = 0;
        /**
         * Returns node material.
         *
         * @return Material.
         */
        virtual Material* getMaterial() const = 0;
    };
}

#endif
