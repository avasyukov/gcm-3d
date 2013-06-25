#ifndef GCM_DISPATCHER_H_
#define GCM_DISPATCHER_H_

//#include "Interfaces.h"
//#include <string>
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
	public:
		GCMDispatcher();
		~GCMDispatcher();
		virtual void setEngine(IEngine* e) = 0;
		virtual void prepare(int numberOfWorkers, AABB* scene) = 0;
		virtual int getOwner(float x, float y, float z) = 0;
		virtual int getOwner(float coords[3]) = 0;
		virtual bool isMine(float coords[3]) = 0;
		virtual bool isMine(double coords[3]) = 0;
		virtual AABB* getOutline(int index) = 0;
		virtual void printZones() = 0;
	};
}

#endif