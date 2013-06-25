#ifndef GCM_ENGINE_H
#define GCM_ENGINE_H

#include <string>
#include <stdexcept>
#include <vector>
#include <mpi.h>
#include <limits>

#include "Logging.h"
#include "Body.h"
#include "Interfaces.h"
#include "mesh/MeshLoader.h"
#include "mesh/MshMeshLoader.h"
#include "mesh/Msh2MeshLoader.h"
#include "mesh/GeoMeshLoader.h"
#include "mesh/Geo2MeshLoader.h"
#include "mesh/VtuMeshLoader.h"
#include "mesh/Vtu2MeshLoader.h"
#include "node/NodeFactory.h"
#include "node/ElasticNodeFactory.h"
#include "snapshot/SnapshotWriter.h"
#include "Utils.h"
#include "method/NumericalMethod.h"
#include "method/InterpolationFixedAxis.h"
#include "calc/volume/VolumeCalculator.h"
#include "calc/volume/SimpleVolumeCalculator.h"
#include "calc/border/BorderCalculator.h"
#include "calc/border/FreeBorderCalculator.h"
#include "calc/border/SmoothBorderCalculator.h"
#include "calc/contact/ContactCalculator.h"
#include "rheology/RheologyCalculator.h"
#include "util/forms/StepPulseForm.h"
#include "util/areas/BoxArea.h"
#include "GCMDispatcher.h"
#include "DummyDispatcher.h"
#include "DataBus.h"
#include "snapshot/VTKSnapshotWriter.h"

// FIXME
#include "interpolator/TetrInterpolator.h"
#include "interpolator/TetrFirstOrderInterpolator.h"
#include "interpolator/TetrSecondOrderMinMaxInterpolator.h"

using namespace std;
using namespace gcm;

namespace gcm
{
	/*
	 * Main class to operate calculation scene.
	 */
	class Engine: public IEngine {
	protected:
		/*
		 * Engines counter.
		 */
		static int enginesNumber;
		/*
		 * Process rank in MPI communicator.
		 */
		int rank;
		int numberOfWorkers;
		/*
		 * Mesh loaders.
		 */
		map<string, MeshLoader*> meshLoaders;
		/*
		 * Snapshot writers.
		 */
		map<string, SnapshotWriter*> snapshotWriters;
		/*
		 * Node factories.
		 */
		map<string, NodeFactory*> nodeFactories;
		/*
		 * Numerical methods
		 */
		map<string, NumericalMethod*> numericalMethods;
		/*
		 * Volume calculators
		 */
		map<string, VolumeCalculator*> volumeCalculators;
		/*
		 * Border calculators
		 */
		map<string, BorderCalculator*> borderCalculators;
		/*
		 * Contact calculators
		 */
		map<string, ContactCalculator*> contactCalculators;
		
		map<string, RheologyCalculator*> rheologyCalculators;
		
		// FIXME
		map<string, TetrInterpolator*> interpolators;
		
		vector<BorderCondition*> borderConditions;
		/*
		 * Scene bodies.
		 */
		vector<Body*> bodies;
		
		gcm::GCMDispatcher* dispatcher;
		
		DataBus* dataBus;
		
		float fixedTimeStep;
		float currentTime;
		
		int numberOfSnaps;
		int stepsPerSnap;
		/*
		 * Logger.
		 */
		USE_LOGGER;
	public:
		AABB scene;
		/*
		 * Default constructor. Engine uses log4cxx as logging subsystem, so
		 * it must be initialized *before* creating engine instance.
		 */
		Engine();
		/*
		 * Constructor to create scene taking into account
		 * command line arguments.
		 */
		Engine(int *argc, char ***argv);
		/*
		 * Destructor.
		 */
		~Engine();
		/*
		 * Returns process rank.
		 */
		int getRank();
		int getNumberOfWorkers();
		int getNumberOfBodies();
		/*
		 * Performs calculation of one time step. If step is omitted
		 * then actual time step is determined automatically.
		 */
		void doStep(float step = 0.0);
		/*
		 * Registers new mesh loader. Out-of-box mesh loaders are
		 * registered automatically at engine creation. Note, that if mesh
		 * loader with the same type is registered already it will be replaced.
		 */
		void registerMeshLoader(MeshLoader *meshLoader);
		/*
		 * Registers new snapshot writer. Out-of-box snapshot writers are
		 * registered automatically at engine creation. Note, that if snapshot
		 * writer with the same type is registered already it will be replaced.
		 */
		void registerSnapshotWriter(SnapshotWriter *snapshotWriter);
		/*
		 * Registers new node factory. Out-of-box node factories are
		 * registered automatically at engine creation. Note, that if node factory
		 * with the same type is registered already it will be replaced.
		 */
		void registerNodeFactory(NodeFactory *nodeFactory);
		/*
		 * Registers new volume calculator. Out-of-box calculators are
		 * registered automatically at engine creation. Note, that if calculator
		 * with the same type is registered already it will be replaced.
		 */
		void registerVolumeCalculator(VolumeCalculator *volumeCalculator);
		/*
		 * Registers new border calculator. Out-of-box calculators are
		 * registered automatically at engine creation. Note, that if calculator
		 * with the same type is registered already it will be replaced.
		 */
		void registerBorderCalculator(BorderCalculator *borderCalculator);
		/*
		 * Registers new contact calculator. Out-of-box calculators are
		 * registered automatically at engine creation. Note, that if calculator
		 * with the same type is registered already it will be replaced.
		 */
		void registerContactCalculator(ContactCalculator *contactCalculator);
		
		void registerNumericalMethod(NumericalMethod *numericalMethod);
		
		void registerInterpolator(TetrInterpolator *interpolator);
		
		void registerRheologyCalculator(RheologyCalculator *rheologyCalculator);
		
		void addBorderCondition(BorderCondition *borderCondition);
		
		/*
		 * Returns mesh loader by type or NULL if not found.
		 */
		MeshLoader* getMeshLoader(string type);
		/*
		 * Returns snapshot writer by type or NULL if not found.
		 */
		SnapshotWriter* getSnapshotWriter(string type);
		/*
		 * Returns node factory by type or NULL if not found.
		 */
		NodeFactory* getNodeFactory(string type);
		
		NumericalMethod* getNumericalMethod(string type);
		VolumeCalculator* getVolumeCalculator(string type);
		BorderCalculator* getBorderCalculator(string type);
		ContactCalculator* getContactCalculator(string type);
		BorderCondition* getBorderCondition(unsigned int num);
		TetrFirstOrderInterpolator* getFirstOrderInterpolator(string type);
		TetrSecondOrderMinMaxInterpolator* getSecondOrderInterpolator(string type);
		RheologyCalculator* getRheologyCalculator(string type);
		GCMDispatcher* getDispatcher();
		
		/*
		 * Returns body object by its id or NULL if not found
		 */
		Body* getBodyById(string id);
		
		Body* getBody(unsigned int num);
		/*
		 * Adds new body to scene.
		 */
		void addBody(Body *body);
		
		void doNextStep();
		void doNextStepStages( const float maxAllowedStep, float& actualTimeStep );
		void doNextStepAfterStages (const float time_step);
		
		void setTimeStep(float dt);
		float getTimeStep();
		float getCurrentTime();
		void setCurrentTime(float time);
		void syncNodes();
		void syncOutlines();
		void calculate();
		void setNumberOfSnaps(int number);
		void setStepsPerSnap(int number);

		DataBus* getDataBus();
	};
}

#endif
