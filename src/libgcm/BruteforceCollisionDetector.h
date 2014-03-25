#ifndef _GCM_Bruteforce_COLLISION_DETECTOR
#define _GCM_Bruteforce_COLLISION_DETECTOR 1

#include "CollisionDetector.h"
#include "Math.h"

using namespace std;

namespace gcm {

    class BruteforceCollisionDetector : public CollisionDetector
    {
    public:
        BruteforceCollisionDetector();
        ~BruteforceCollisionDetector();
        string getType () const;
        void find_collisions(vector<CalcNode> &virt_nodes);
    };

}

#endif
