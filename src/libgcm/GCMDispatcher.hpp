#ifndef GCM_DISPATCHER_H_
#define GCM_DISPATCHER_H_

#include <string>

#include "libgcm/util/AABB.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {

    class GCMDispatcher {
    private:
        USE_LOGGER;
    protected:
        float dX, dY, dZ;
        std::string myBodyId;
        std::map<std::string,AABB> bodiesOutlines;
        std::map<std::string,int> bodiesSlicingDirections;
        std::map<std::string,int> bodiesNodesNumbers;
    public:
        GCMDispatcher();
        virtual ~GCMDispatcher() = 0;
        virtual void prepare(int numberOfWorkers, AABB* scene) = 0;
        virtual int getOwner(float x, float y, float z) = 0;
        virtual int getOwner(float coords[3]) = 0;
        virtual int getOwner(float x, float y, float z, std::string bodyId) = 0;
        virtual int getOwner(float coords[3], std::string bodyId) = 0;
        virtual bool isMine(float coords[3], std::string bodyId) = 0;
        virtual bool isMine(double coords[3], std::string bodyId) = 0;
        virtual AABB* getOutline(int index) = 0;
        virtual void printZones() = 0;
        inline std::string getMyBodyId()
        {
            return myBodyId;
        }
        inline void addBodyOutline(std::string id, AABB outline)
        {
            bodiesOutlines[id] = outline;
        }
        inline AABB getBodyOutline(std::string id)
        {
            return bodiesOutlines[id];
        }
        inline void addBodySlicingDirection(std::string id, int dir)
        {
            bodiesSlicingDirections[id] = dir;
        }
        inline int getBodySlicingDirection(std::string id)
        {
            return bodiesSlicingDirections[id];
        }
        inline void addBodyNodesNumber(std::string id, int num)
        {
            bodiesNodesNumbers[id] = num;
        }
        inline int getBodyNodesNumber(std::string id)
        {
            return bodiesNodesNumbers[id];
        }
        inline void setTransferVector(float x, float y, float z, std::string bodyId)
        {
            LOG_DEBUG("setTransferVector called. Bodies: " << bodyId << " " << myBodyId << " Vector: " << x << " " << y << " " << z << "\n");
            if( (bodyId != myBodyId ) && (myBodyId != "__any") )
                return;
            dX = x;
            dY = y;
            dZ = z;
        }
    };
}

#endif
