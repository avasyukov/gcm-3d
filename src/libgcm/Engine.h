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
#include "snapshot/SnapshotWriter.h"
#include "Utils.h"
#include "method/NumericalMethod.h"
#include "calc/volume/VolumeCalculator.h"
#include "calc/border/BorderCalculator.h"
#include "calc/contact/ContactCalculator.h"
#include "rheology/RheologyCalculator.h"
#include "util/forms/PulseForm.h"
#include "util/areas/BoxArea.h"
#include "GCMDispatcher.h"
#include "DummyDispatcher.h"
#include "DataBus.h"
#include "Material.h"
#include "BorderCondition.h"

// FIXME - do we need it here?
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
		 * File lookup service
		 */
		 FileLookupService fls;
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
		
		// FIXME - tetr-specific is bad
		map<string, TetrInterpolator*> interpolators;
		
		vector<BorderCondition*> borderConditions;
		
		vector<Material*> materials;
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
		
		AABB scene;
		/*
		 * Logger.
		 */
		USE_LOGGER;
		
	private:
		/* 
		 * Engine is a singletone, so constructors are private
		 * Limitation of current design: single threaded only
		 */ 
		
		/*
		 * Default constructor. Engine uses log4cxx as logging subsystem, so
		 * it must be initialized *before* creating engine instance.
		 */
		Engine();
		Engine(Engine const& copy);	// Not implemented
		Engine& operator=(Engine const& copy); //Not implemented
		/*
		 * Destructor.
		 */
		~Engine();
		
	public:
		/*
		 * Returns singletone engine instance
		 */
		static Engine& getInstance();
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
		
		unsigned int addBorderCondition(BorderCondition *borderCondition);
		void replaceDefaultBorderCondition(BorderCondition *borderCondition);
		
		unsigned char addMaterial(Material *material);
		
		/*
		 * Returns mesh loader by type or NULL if not found.
		 */
		MeshLoader* getMeshLoader(string type);
		/*
		 * Returns snapshot writer by type or NULL if not found.
		 */
		SnapshotWriter* getSnapshotWriter(string type);
		
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
		
		unsigned char getMaterialIndex(string id);
		Material* getMaterial(string id);
		Material* getMaterial(unsigned char index);
		
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
		AABB getScene();
		void setScene(AABB src);
		void transferScene(float x, float y, float z);
		
		DataBus* getDataBus();

		FileLookupService& getFileLookupService();
	};
}

#endif
