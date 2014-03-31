/*
 * File:   TetrSecondOrderMinMaxInterpolator.h
 * Author: anganar
 *
 * Created on May 3, 2013, 12:13 AM
 */

#ifndef GCM_TETR_SECOND_ORDER_MINMAX_INTERPOLATOR_H
#define    GCM_TETR_SECOND_ORDER_MINMAX_INTERPOLATOR_H

#include <string>

#include "libgcm/Math.h"
#include "libgcm/Logging.h"
#include "libgcm/interpolator/TetrInterpolator.h"

using namespace std;
using namespace gcm;

namespace gcm {
    class CalcNode;

    class TetrSecondOrderMinMaxInterpolator : public TetrInterpolator {
    public:
        TetrSecondOrderMinMaxInterpolator();
        ~TetrSecondOrderMinMaxInterpolator();
        void interpolate( CalcNode& node,
                CalcNode& node0, CalcNode& node1, CalcNode& node2, CalcNode& node3,
                CalcNode& addNode0, CalcNode& addNode1, CalcNode& addNode2,
                CalcNode& addNode3, CalcNode& addNode4, CalcNode& addNode5 );
    private:
        CalcNode* baseNodes[4];
        CalcNode* addNodes[6];
        USE_LOGGER;
    };
}

#endif    /* GCM_TETR_FIRST_ORDER_INTERPOLATOR_H */

