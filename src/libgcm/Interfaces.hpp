#ifndef GCM_INTERFACES_H
#define GCM_INTERFACES_H

#include <string>

#include <mpi.h>

using namespace std;

namespace gcm {
    class Area;
    class AABB;
    class VolumeCalculator;
    class BorderCalculator;
    class ContactCalculator;
    class BorderCondition;
    class ContactCondition;
    class RheologyCalculator;
    class TetrFirstOrderInterpolator;
    class TetrSecondOrderMinMaxInterpolator;
    class LineFirstOrderInterpolator;
    class GCMDispatcher;
    class Body;
    class Mesh;
    class NumericalMethod;
    class CalcNode;
    //class GCMDispatcher;


    class IBody {
        public:
            virtual ~IBody() = 0;
            virtual string getId() = 0;
            virtual Mesh* getMesh(string id) = 0;
            virtual void setInitialState(Area* area, float values[9]) = 0;
            virtual string getRheologyCalculatorType() = 0;
            virtual void setRheologyCalculatorType(string calcType) = 0;
    };
}

#endif
