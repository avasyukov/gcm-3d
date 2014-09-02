#ifndef GCM_ZeroDeviatorFailedMaterialCorrector_H
#define GCM_ZeroDeviatorFailedMaterialCorrector_H

#include "libgcm/failure/correctors/FailedMaterialCorrector.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class ZeroDeviatorFailedMaterialCorrector : public FailedMaterialCorrector {
    public:
		ZeroDeviatorFailedMaterialCorrector();
		void applyCorrection(ICalcNode& node);
    private:
        USE_LOGGER;
    };
}

#endif

