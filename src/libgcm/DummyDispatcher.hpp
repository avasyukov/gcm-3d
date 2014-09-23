#ifndef GCM_DUMMY_DISPATCHER_H_
#define GCM_DUMMY_DISPATCHER_H_

#include <stdexcept>

#include "libgcm/GCMDispatcher.hpp"


namespace gcm {

    class DummyDispatcher : public GCMDispatcher {
    private:
        USE_LOGGER;
        AABB* outlines;
        int outlinesNum;
        int myNumberOfWorkers;
        int rank;
        int* workersPerBody;
        int distributionIsOk();
        void adjustDistribution();
        int getLeastComputedBody();
        int getMostComputedBody();
    public:
        DummyDispatcher();
        ~DummyDispatcher();
        void prepare(int numberOfWorkers, AABB* scene);
        int getOwner(float x, float y, float z);
        int getOwner(float coords[3]);
        int getOwner(float x, float y, float z, std::string bodyId);
        int getOwner(float coords[3], std::string bodyId);
        bool isMine(float coords[3], std::string bodyId);
        bool isMine(double coords[3], std::string bodyId);
        AABB* getOutline(int index);
        void printZones();
    };
}

#endif
