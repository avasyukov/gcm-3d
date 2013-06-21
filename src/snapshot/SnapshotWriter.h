#ifndef GCM_SNAPSHOTWRITER_H_
#define GCM_SNAPSHOTWRITER_H_

#include <string>
#include "../mesh/TetrMeshFirstOrder.h"

using namespace std;
using namespace gcm;

namespace gcm {
	class SnapshotWriter {
	public:
		/*
		 * Returns snapshot writer type
		 */
		virtual string getType() = 0;
		virtual void dump(TetrMeshFirstOrder* mesh, int step) = 0;
		virtual void init() = 0;
	};
}


#endif
