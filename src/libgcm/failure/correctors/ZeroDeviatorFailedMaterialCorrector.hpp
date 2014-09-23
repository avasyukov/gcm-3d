#ifndef GCM_ZeroDeviatorFailedMaterialCorrector_H
#define GCM_ZeroDeviatorFailedMaterialCorrector_H

#include "libgcm/failure/correctors/FailedMaterialCorrector.hpp"


namespace gcm {

    class ZeroDeviatorFailedMaterialCorrector : public FailedMaterialCorrector {
    public:
		ZeroDeviatorFailedMaterialCorrector();
		void applyCorrection(ICalcNode& node, const float tau);
    private:
        USE_LOGGER;
    };
}

#endif

