#ifndef GCM_INTERPOLATOR_H
#define	GCM_INTERPOLATOR_H

#include <string>
#include "../Math.h"
#include "../Logging.h"

using namespace std;
using namespace gcm;

namespace gcm {

	class TetrInterpolator {
	public:
		TetrInterpolator();
		~TetrInterpolator();
		string getType();
	protected:
		string type;
	private:
		USE_LOGGER;
	};
}

#endif

