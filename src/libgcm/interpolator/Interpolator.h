#ifndef GCM_INTERPOLATOR_H
#define	GCM_INTERPOLATOR_H

#include <string>
#include "../node/ElasticNode.h"
#include "../Math.h"
#include "../Logging.h"

using namespace std;
using namespace gcm;

namespace gcm {

	class Interpolator {
	public:
		Interpolator();
		~Interpolator();
		string getType();
	protected:
		string type;
	private:
		USE_LOGGER;
	};
}

#endif
