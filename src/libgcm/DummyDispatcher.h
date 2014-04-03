#ifndef GCM_DUMMY_DISPATCHER_H_
#define GCM_DUMMY_DISPATCHER_H_

#include <stdexcept>

#include "Interfaces.h"
#include "GCMDispatcher.h"

using namespace std;
using namespace gcm;

namespace gcm {

    class DummyDispatcher : public GCMDispatcher {
    private:
        USE_LOGGER;
        IEngine* engine;
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
        void setEngine(IEngine* e);
        void prepare(int numberOfWorkers, AABB* scene);
        int getOwner(float x, float y, float z);
        int getOwner(float coords[3]);
        int getOwner(float x, float y, float z, string bodyId);
        int getOwner(float coords[3], string bodyId);
        bool isMine(float coords[3], string bodyId);
        bool isMine(double coords[3], string bodyId);
        AABB* getOutline(int index);
        void printZones();
    };
}

#endif
