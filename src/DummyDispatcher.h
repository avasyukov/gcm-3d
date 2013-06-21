#ifndef GCM_DUMMY_DISPATCHER_H_
#define GCM_DUMMY_DISPATCHER_H_

#include "Interfaces.h"
//#include <string>
//#include "util/AABB.h"
//#include "Logging.h"
#include "GCMDispatcher.h"
//#include "Interfaces.h"
//#include "DataBus.h"

using namespace std;
using namespace gcm;

namespace gcm {

	class DummyDispatcher : public GCMDispatcher {
	private:
		USE_LOGGER;
		IEngine* engine;
		AABB* outlines;
		int outlinesNum;
		int rank;
	public:
		DummyDispatcher();
		~DummyDispatcher();
		void setEngine(IEngine* e);
		void prepare(int numberOfWorkers, AABB* scene);
		int getOwner(float x, float y, float z);
		int getOwner(float coords[3]);
		bool isMine(float coords[3]);
		bool isMine(double coords[3]);
		AABB* getOutline(int index);
		void printZones();
	};
}

#endif