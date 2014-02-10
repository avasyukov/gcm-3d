#ifndef GCM_INTERFACES_H
#define GCM_INTERFACES_H

#include <string>

#include <mpi.h>

#include "Utils.h"

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
	/*
	 * Helper interfaces to solve compilation problems with cross-dependant classes.
	 */
	class IEngine {
		public:
			virtual ~IEngine() = 0;
			virtual NumericalMethod* getNumericalMethod(string type) = 0;
			virtual VolumeCalculator* getVolumeCalculator(string type) = 0;
			virtual BorderCalculator* getBorderCalculator(string type) = 0;
			virtual ContactCalculator* getContactCalculator(string type) = 0;
			virtual BorderCondition* getBorderCondition(unsigned int num) = 0;
			virtual ContactCondition* getContactCondition(unsigned int num) = 0;
			virtual TetrFirstOrderInterpolator* getFirstOrderInterpolator(string type) = 0;
			virtual TetrSecondOrderMinMaxInterpolator* getSecondOrderInterpolator(string type) = 0;
			virtual LineFirstOrderInterpolator* getFirstOrderLineInterpolator(string type) = 0;
			virtual RheologyCalculator* getRheologyCalculator(string type) = 0;
			virtual GCMDispatcher* getDispatcher() = 0;
			virtual void doNextStep() = 0;
			virtual int getRank() = 0;
			virtual int getNumberOfWorkers() = 0;
			virtual int getNumberOfBodies() = 0;
			virtual Body* getBody(unsigned int num) = 0;
			virtual Body* getBodyById(string id) = 0;
			virtual void syncNodes() = 0;
			virtual AABB getScene() = 0;
			virtual void setScene(AABB src) = 0;
			virtual void transferScene(float x, float y, float z) = 0;
			virtual FileLookupService& getFileLookupService() = 0;
			virtual CalcNode* getVirtNode(unsigned int i) = 0;
			virtual float getGmshVerbosity() = 0;
			virtual void setGmshVerbosity(float verbosity) = 0;
	};

	class IBody {
		public:
			virtual ~IBody() = 0;
			virtual string getId() = 0;
			virtual Mesh* getMesh(string id) = 0;
			virtual IEngine* getEngine() = 0;
			virtual void setInitialState(Area* area, float values[9]) = 0;
			virtual string getRheologyCalculatorType() = 0;
			virtual void setRheologyCalculatorType(string calcType) = 0;
	};
}

#endif
