#ifndef GCM_INTERFACES_H
#define GCM_INTERFACES_H

#include <string>
//#include "mesh/Mesh.h"
#include "node/NodeFactory.h"
//#include "method/NumericalMethod.h"

#include "calc/volume/VolumeCalculator.h"
#include "calc/border/BorderCalculator.h"
#include "calc/contact/ContactCalculator.h"
#include "BorderCondition.h"
#include "rheology/RheologyCalculator.h"
#include "interpolator/TetrFirstOrderInterpolator.h"
#include "interpolator/TetrSecondOrderMinMaxInterpolator.h"
#include "GCMDispatcher.h"

namespace gcm {
	class Body;
	class Mesh;
	class NumericalMethod;
	class NodeFactory;
	//class GCMDispatcher;
	/*
	 * Helper interfaces to solve compilation problems with cross-dependant classes.
	 */
	class IEngine {
		public:
			virtual NodeFactory* getNodeFactory(string type) = 0;
			virtual NumericalMethod* getNumericalMethod(string type) = 0;
			virtual VolumeCalculator* getVolumeCalculator(string type) = 0;
			virtual BorderCalculator* getBorderCalculator(string type) = 0;
			virtual ContactCalculator* getContactCalculator(string type) = 0;
			virtual BorderCondition* getBorderCondition(unsigned int num) = 0;
			virtual TetrFirstOrderInterpolator* getFirstOrderInterpolator(string type) = 0;
			virtual TetrSecondOrderMinMaxInterpolator* getSecondOrderInterpolator(string type) = 0;
			virtual RheologyCalculator* getRheologyCalculator(string type) = 0;
			virtual GCMDispatcher* getDispatcher() = 0;
			virtual void doNextStep() = 0;
			virtual int getRank() = 0;
			virtual int getNumberOfWorkers() = 0;
			virtual int getNumberOfBodies() = 0;
			virtual Body* getBody(unsigned int num) = 0;
			virtual void syncNodes() = 0;
			virtual AABB getScene() = 0;
			virtual void setScene(AABB src) = 0;
			virtual void transferScene(float x, float y, float z) = 0;
	};

	class IBody {
		public:
			virtual Mesh* getMesh(string id) = 0;
			virtual IEngine* getEngine() = 0;
			virtual string getRheology() = 0;
	};
}

#endif