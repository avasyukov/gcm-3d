#ifndef _GCM_Bruteforce_COLLISION_DETECTOR
#define _GCM_Bruteforce_COLLISION_DETECTOR 1

#include "libgcm/CollisionDetector.hpp"
#include "libgcm/Math.hpp"


namespace gcm {

    class BruteforceCollisionDetector : public CollisionDetector
    {
    public:
        BruteforceCollisionDetector();
        ~BruteforceCollisionDetector();
        std::string getType () const;
        void find_collisions(std::vector<CalcNode> &virt_nodes);
    };

}

#endif
