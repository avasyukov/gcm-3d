#ifndef GCM_LINE_SECOND_ORDER_INTERPOLATOR_H
#define    GCM_LINE_FIRST_ORDER_INTERPOLATOR_H

#include <string>

#include "libgcm/Math.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/interpolator/TetrInterpolator.hpp"

using namespace std;
using namespace gcm;

namespace gcm {
    class CalcNode;

    class LineSecondOrderInterpolator : public TetrInterpolator {
    public:
        LineSecondOrderInterpolator();
        ~LineSecondOrderInterpolator();
        void interpolate( CalcNode& node, CalcNode& nodeLeft, CalcNode& nodeCur, CalcNode& nodeRight );
    private:
        USE_LOGGER;
    };
}

#endif    /* GCM_LINE_SECOND_ORDER_INTERPOLATOR_H */

