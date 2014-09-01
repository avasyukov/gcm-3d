#ifndef GCM_DummyFailedMaterialCorrector_H
#define GCM_DummyFailedMaterialCorrector_H

#include "libgcm/failure/correctors/FailedMaterialCorrector.hpp"

using namespace std;
using namespace gcm;

namespace gcm {

    class DummyFailedMaterialCorrector : public FailedMaterialCorrector {
    public:
		DummyFailedMaterialCorrector();
		void applyCorrection(ICalcNode& node);
    private:
        USE_LOGGER;
    };
}

#endif

