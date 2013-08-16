#ifndef GCM_DISPATCHER_H_
#define GCM_DISPATCHER_H_

//#include "Interfaces.h"
#include <string>
#include "util/AABB.h"
#include "Logging.h"
//#include "Engine.h"

using namespace std;
//using namespace gcm;

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
	public:
		GCMDispatcher();
		~GCMDispatcher();
		virtual void setEngine(IEngine* e) = 0;
		virtual void prepare(int numberOfWorkers, AABB* scene) = 0;
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
		inline void setTransferVector(float x, float y, float z, string bodyId)
		{
			if( bodyId != myBodyId )
				return;
			dX = x;
			dY = y;
			dZ = z;
		}
	};
}

#endif