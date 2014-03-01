#ifndef GCM_DISPATCHER_H_
#define GCM_DISPATCHER_H_

#include <string>

#include "util/AABB.h"
#include "Logging.h"

using namespace std;

namespace gcm {

	class IEngine;
	
	class GCMDispatcher {
	private:
		USE_LOGGER;
		IEngine* engine;
	protected:
		float dX, dY, dZ;
		string myBodyId;
		map<string,AABB> bodiesOutlines;
		map<string,int> bodiesSlicingDirections;
		map<string,int> bodiesNodesNumbers;
	public:
		GCMDispatcher();
		virtual ~GCMDispatcher() = 0;
		virtual void setEngine(IEngine* e) = 0;
		virtual void prepare(int numberOfWorkers, AABB* scene) = 0;
		virtual int getOwner(float x, float y, float z) = 0;
		virtual int getOwner(float coords[3]) = 0;
		virtual int getOwner(float x, float y, float z, string bodyId) = 0;
		virtual int getOwner(float coords[3], string bodyId) = 0;
		virtual bool isMine(float coords[3], string bodyId) = 0;
		virtual bool isMine(double coords[3], string bodyId) = 0;
		virtual AABB* getOutline(int index) = 0;
		virtual void printZones() = 0;
		inline string getMyBodyId()
		{
			return myBodyId;
		}
		inline void addBodyOutline(string id, AABB outline)
		{
			bodiesOutlines[id] = outline;
		}
		inline AABB getBodyOutline(string id)
		{
			return bodiesOutlines[id];
		}
		inline void addBodySlicingDirection(string id, int dir)
		{
			bodiesSlicingDirections[id] = dir;
		}
		inline int getBodySlicingDirection(string id)
		{
			return bodiesSlicingDirections[id];
		}
		inline void addBodyNodesNumber(string id, int num)
		{
			bodiesNodesNumbers[id] = num;
		}
		inline int getBodyNodesNumber(string id)
		{
			return bodiesNodesNumbers[id];
		}
		inline void setTransferVector(float x, float y, float z, string bodyId)
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
