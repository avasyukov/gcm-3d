#include "Engine.h"
#include "mesh/MshMeshLoader.h"
#include "mesh/Msh2MeshLoader.h"
#include "mesh/GeoMeshLoader.h"
#include "mesh/Geo2MeshLoader.h"
#include "mesh/VtuMeshLoader.h"
#include "mesh/Vtu2MeshLoader.h"
#include "mesh/Vtu2MeshZoneLoader.h"
#include "method/InterpolationFixedAxis.h"
#include "calc/volume/SimpleVolumeCalculator.h"
#include "calc/border/FreeBorderCalculator.h"
#include "calc/border/SmoothBorderCalculator.h"
#include "calc/contact/SlidingContactCalculator.h"
#include "calc/contact/AdhesionContactCalculator.h"
#include "calc/contact/AdhesionContactDestroyCalculator.h"
#include "util/forms/StepPulseForm.h"
#include "snapshot/VTKSnapshotWriter.h"
#include "snapshot/VTK2SnapshotWriter.h"
#include "BruteforceCollisionDetector.h"
#include "rheology/DummyRheologyCalculator.h"
#include "rheology/StdRheologyCalculator.h"

// initialiaze static fields
int gcm::Engine::enginesNumber = 0;
Engine* gcm::Engine::engineInstance = 0;

Engine& gcm::Engine::getInstance()
{
	if (!engineInstance) initInstance();
	return *engineInstance;
}

gcm::Engine::Engine()
{
	// check engines counter
	if (enginesNumber)
	{
		// because of some internal architecture limitations we support
		// only one instance of engine at the same time
		throw logic_error("Only one engine can be used at the same time");
	}
	engineInstance = this;
	enginesNumber++;
	// init MPI subsystem used for IPC
	if( ! MPI::Is_initialized() )
		MPI::Init();
	rank = MPI::COMM_WORLD.Get_rank();
	numberOfWorkers = MPI::COMM_WORLD.Get_size();
	// get logger
	INIT_LOGGER("gcm.Engine");
	LOG_DEBUG("GCM engine created");
	LOG_DEBUG("Setting default engine values");
	LOG_DEBUG("Registering default mesh loaders");
	registerMeshLoader(new MshMeshLoader());
	registerMeshLoader(new Msh2MeshLoader());
	registerMeshLoader(new GeoMeshLoader());
	registerMeshLoader(new Geo2MeshLoader());
	registerMeshLoader(new VtuMeshLoader());
	registerMeshLoader(new Vtu2MeshLoader());
	registerMeshLoader(new Vtu2MeshZoneLoader());
	LOG_DEBUG("Registering default methods");
	registerNumericalMethod( new InterpolationFixedAxis() );
	LOG_DEBUG("Registering default interpolators");
	registerInterpolator( new TetrFirstOrderInterpolator() );
	registerInterpolator( new TetrSecondOrderMinMaxInterpolator() );
	LOG_DEBUG("Registering default rheology calculators");
	registerRheologyCalculator( new DummyRheologyCalculator() );
	registerRheologyCalculator( new StdRheologyCalculator() );
	defaultRheoCalcType = "DummyRheologyCalculator";
	LOG_DEBUG("Registering default calculators");
	registerVolumeCalculator( new SimpleVolumeCalculator() );
	registerBorderCalculator( new FreeBorderCalculator() );
	registerBorderCalculator( new SmoothBorderCalculator() );
	registerContactCalculator( new SlidingContactCalculator() );
	registerContactCalculator( new AdhesionContactCalculator() );
	registerContactCalculator( new AdhesionContactDestroyCalculator() );
	LOG_DEBUG("Registering default border condition");
	// Failsafe border condition
	addBorderCondition( new BorderCondition( NULL, new StepPulseForm(-1, -1), getBorderCalculator("SmoothBorderCalculator") ) );
	// Default border condition
	addBorderCondition( new BorderCondition( NULL, new StepPulseForm(-1, -1), getBorderCalculator("FreeBorderCalculator") ) );
	LOG_DEBUG("Registering default contact condition");
	addContactCondition( new ContactCondition( NULL, new StepPulseForm(-1, -1), getContactCalculator("SlidingContactCalculator") ) );
	LOG_DEBUG("Creating dispatcher");
	dispatcher = new DummyDispatcher();
	dispatcher->setEngine(this);
	LOG_DEBUG("Creating data bus");
	dataBus = new DataBus();
	dataBus->setEngine(this);
	LOG_DEBUG("Creating snapshot writers");
	vtkSnapshotWriter = new VTKSnapshotWriter();
	vtkDumpWriter = new VTK2SnapshotWriter();
	LOG_DEBUG("Creating collision detector");
	colDet = new BruteforceCollisionDetector();
	LOG_INFO("GCM engine initialized");
	currentTime = 0;
	currentTimeStep = 0;
	fixedTimeStep = -1;
	numberOfSnaps = 0;
	stepsPerSnap = 1;
	contactThresholdType = CONTACT_THRESHOLD_BY_AVG_H;
	contactThresholdFactor = 1.0;
}

gcm::Engine::~Engine()
{
	cleanUp();
	LOG_INFO("GCM engine destroyed");
}

void gcm::Engine::cleanUp()
{
	// clear memory
	for(auto& b: bodies)
		delete b;
	for(auto& ml: meshLoaders)
	{
		(ml.second)->cleanUp();
		delete (ml.second);
	}
	// decrement engines counter
	enginesNumber--;
	delete dataBus;
	delete vtkSnapshotWriter;
	delete vtkDumpWriter;
	delete colDet;
	// shutdown MPI
	if( !MPI::Is_finalized() )
		MPI::Finalize();
	LOG_INFO("Clean up done");
}

int gcm::Engine::getRank()
{
	return rank;
}

int gcm::Engine::getNumberOfWorkers()
{
	return numberOfWorkers;
}

void gcm::Engine::setTimeStep(float dt)
{
	fixedTimeStep = dt;
}

float gcm::Engine::getTimeStep()
{
	return fixedTimeStep;
}

GCMDispatcher* gcm::Engine::getDispatcher()
{
	return dispatcher;
}

void gcm::Engine::registerMeshLoader(MeshLoader* meshLoader)
{
	if (!meshLoader)
		THROW_INVALID_ARG("Mesh loader parameter cannot be NULL");
	meshLoaders[meshLoader->getType()] = meshLoader;
	meshLoader->setEngine(this);
	LOG_DEBUG("Registered mesh loader: " << meshLoader->getType());
}

void gcm::Engine::registerSnapshotWriter(SnapshotWriter* snapshotWriter)
{
	if (!snapshotWriter)
		THROW_INVALID_ARG("Snapshot writer parameter cannot be NULL");
	snapshotWriters[snapshotWriter->getType()] = snapshotWriter;
	LOG_DEBUG("Registered snapshot writer: " << snapshotWriter->getType());
}

void gcm::Engine::registerVolumeCalculator(VolumeCalculator *volumeCalculator)
{
	if (!volumeCalculator)
		THROW_INVALID_ARG("Volume calculator parameter cannot be NULL");
	volumeCalculators[volumeCalculator->getType()] = volumeCalculator;
	LOG_DEBUG("Registered volume calculator: " << volumeCalculator->getType());
}

void gcm::Engine::registerBorderCalculator(BorderCalculator *borderCalculator)
{
	if (!borderCalculator)
		THROW_INVALID_ARG("Border calculator parameter cannot be NULL");
	borderCalculators[borderCalculator->getType()] = borderCalculator;
	LOG_DEBUG("Registered border calculator: " << borderCalculator->getType());
}

void gcm::Engine::registerContactCalculator(ContactCalculator *contactCalculator)
{
	if (!contactCalculator)
		THROW_INVALID_ARG("Contact calculator parameter cannot be NULL");
	contactCalculators[contactCalculator->getType()] = contactCalculator;
	LOG_DEBUG("Registered contact calculator: " << contactCalculator->getType());
}

void gcm::Engine::registerNumericalMethod(NumericalMethod *numericalMethod)
{
	if (!numericalMethod)
		THROW_INVALID_ARG("Numerical method parameter cannot be NULL");
	numericalMethods[numericalMethod->getType()] = numericalMethod;
	LOG_DEBUG("Registered numerical method: " << numericalMethod->getType());
}

void gcm::Engine::registerInterpolator(TetrInterpolator* interpolator)
{
	if (!interpolator)
		THROW_INVALID_ARG("Interpolator parameter cannot be NULL");
	interpolators[interpolator->getType()] = interpolator;
	LOG_DEBUG("Registered interpolator: " << interpolator->getType());	
}

void gcm::Engine::registerRheologyCalculator(RheologyCalculator* rheologyCalculator)
{
	if (!rheologyCalculator)
		THROW_INVALID_ARG("Rheology calculator parameter cannot be NULL");
	rheologyCalculators[rheologyCalculator->getType()] = rheologyCalculator;
	LOG_DEBUG("Registered rheology calculator: " << rheologyCalculator->getType());	
}

unsigned char gcm::Engine::addMaterial(Material* material)
{
	if( !material )
		THROW_INVALID_ARG("Material parameter cannot be NULL");
	materials.push_back(material);
	unsigned char index = materials.size() - 1;
	LOG_DEBUG("Added new material. Id: " << material->getId() << " Index: " << (int)index);
	return index;
}

unsigned int gcm::Engine::addBorderCondition(BorderCondition *borderCondition)
{
	if (!borderCondition)
		THROW_INVALID_ARG("Border condition parameter cannot be NULL");
	borderConditions.push_back(borderCondition);
	LOG_DEBUG("Added new border condition.");
	return borderConditions.size () - 1;
}

void gcm::Engine::replaceDefaultBorderCondition(BorderCondition *borderCondition)
{
	assert( borderConditions.size() > 1 );
	if (!borderCondition)
		THROW_INVALID_ARG("Border condition parameter cannot be NULL");
	borderConditions[1] = borderCondition;
	LOG_DEBUG("Default border condition set");
}

unsigned int gcm::Engine::addContactCondition(ContactCondition *contactCondition)
{
	if (!contactCondition)
		THROW_INVALID_ARG("Contact condition parameter cannot be NULL");
	contactConditions.push_back(contactCondition);
	LOG_DEBUG("Added new contact condition.");
	return contactConditions.size () - 1;
}

void gcm::Engine::replaceDefaultContactCondition(ContactCondition *contactCondition)
{
	assert( contactConditions.size() > 0 );
	if (!contactCondition)
		THROW_INVALID_ARG("Contact condition parameter cannot be NULL");
	contactConditions[0] = contactCondition;
	LOG_DEBUG("Default contact condition set");
}

unsigned char gcm::Engine::getMaterialIndex(string id)
{
	for (unsigned char i = 0; i < materials.size(); i++)
		if (materials[i]->getId() == id)
			return i;
	THROW_INVALID_ARG("Material was not found");
}

Material* gcm::Engine::getMaterial(string id)
{
	for (unsigned char i = 0; i < materials.size(); i++)
		if (materials[i]->getId() == id)
			return materials[i];
	THROW_INVALID_ARG("Material was not found");
}

Material* gcm::Engine::getMaterial(unsigned char index)
{
	assert( index >=0 && index < materials.size() );
	return materials[index];
}

Body* gcm::Engine::getBodyById(string id)
{
	for (size_t i = 0; i < bodies.size(); i++)
		if (bodies[i]->getId() == id)
			return bodies[i];
	LOG_WARN("Body with id '" << id << "' was not found");
	return NULL;
}

int gcm::Engine::getBodyNum(string id)
{
	for (size_t i = 0; i < bodies.size(); i++)
		if (bodies[i]->getId() == id)
			return i;
	LOG_WARN("Body with id '" << id << "' was not found");
	return -1;
}

Body* gcm::Engine::getBody(unsigned int num)
{
	assert( num < bodies.size() );
	return bodies[num];
}

int gcm::Engine::getNumberOfBodies()
{
	return bodies.size();
}

int gcm::Engine::getNumberOfMaterials()
{
	return materials.size();
}

MeshLoader* gcm::Engine::getMeshLoader(string type)
{
	return meshLoaders.find(type) != meshLoaders.end() ? meshLoaders[type] : NULL;
}

SnapshotWriter* gcm::Engine::getSnapshotWriter(string type)
{
	return snapshotWriters.find(type) != snapshotWriters.end() ? snapshotWriters[type] : NULL;
}

NumericalMethod* gcm::Engine::getNumericalMethod(string type)
{
	return numericalMethods.find(type) != numericalMethods.end() ? numericalMethods[type] : NULL;
}

VolumeCalculator* gcm::Engine::getVolumeCalculator(string type)
{
	return volumeCalculators.find(type) != volumeCalculators.end() ? volumeCalculators[type] : NULL;
}

BorderCalculator* gcm::Engine::getBorderCalculator(string type)
{
	return borderCalculators.find(type) != borderCalculators.end() ? borderCalculators[type] : NULL;
}

ContactCalculator* gcm::Engine::getContactCalculator(string type)
{
	return contactCalculators.find(type) != contactCalculators.end() ? contactCalculators[type] : NULL;
}

TetrFirstOrderInterpolator* gcm::Engine::getFirstOrderInterpolator(string type)
{
	return interpolators.find(type) != interpolators.end() ? (TetrFirstOrderInterpolator*) interpolators[type] : NULL;
}

TetrSecondOrderMinMaxInterpolator* gcm::Engine::getSecondOrderInterpolator(string type)
{
	return interpolators.find(type) != interpolators.end() ? (TetrSecondOrderMinMaxInterpolator*) interpolators[type] : NULL;
}

RheologyCalculator* gcm::Engine::getRheologyCalculator(string type)
{
	return rheologyCalculators.find(type) != rheologyCalculators.end() ? rheologyCalculators[type] : NULL;
}

BorderCondition* gcm::Engine::getBorderCondition(unsigned int num)
{
	assert( num < borderConditions.size() );
	return borderConditions[num];
}

ContactCondition* gcm::Engine::getContactCondition(unsigned int num)
{
	assert( num < contactConditions.size() );
	return contactConditions[num];
}

void gcm::Engine::addBody(Body* body)
{
	bodies.push_back(body);
}

CalcNode* gcm::Engine::getVirtNode(int i)
{
	assert( i >=0 && i < virtNodes.size() );
	return &virtNodes[i];
}

void gcm::Engine::doNextStep()
{
	float step;
	doNextStepBeforeStages(numeric_limits<float>::infinity(), step);
	doNextStepStages(step);
	doNextStepAfterStages(step);
	currentTimeStep++;
}

void gcm::Engine::doNextStepBeforeStages(const float maxAllowedStep, float& actualTimeStep)
{
	if( ! colDet->is_static() )
	{
		// Clear virtual nodes
		virtNodes.clear();
		
		// Clear contact state
		for( unsigned int i = 0; i < bodies.size(); i++ )
		{
				LOG_DEBUG("Clear contact state for body " << i );
				Mesh* mesh = bodies[i]->getMeshes();
				mesh->clearContactState();
		}
	}
	
	// Print debug info
	LOG_DEBUG("Preparing next time step");
	LOG_DEBUG("Space zones for parallel calculations:");
	dispatcher->printZones();
	// FIXME - hardcoded name
	NumericalMethod *method = getNumericalMethod("InterpolationFixedAxis");
	LOG_DEBUG( "Number of stages: " << method->getNumberOfStages() );
	
	// Determine allowed time step
	float tau = numeric_limits<float>::infinity();
	if( fixedTimeStep > 0 ) {
		tau = fixedTimeStep;
	} else {
		tau = calculateRecommendedTimeStep();
	}
	LOG_DEBUG( "Local time step " << tau );
	
	// Sync time step across processes, get final value
	if (tau > maxAllowedStep) tau = maxAllowedStep;
	dataBus->syncTimeStep(&tau);
	actualTimeStep = tau;
	LOG_INFO("Starting step "<< currentTimeStep << ". Current time: " << currentTime << ". " 
				<< "Time step: " << tau << ".");
	
	// Set contact threshold
	colDet->set_threshold( calculateRecommendedContactTreshold(tau) );

	// Sync remote data
	LOG_DEBUG("Syncing outlines");
	dataBus->syncOutlines();
	LOG_DEBUG("Syncing outlines done");
	
	LOG_DEBUG("Syncing remote nodes");
	dataBus->syncNodes(tau);
	LOG_DEBUG("Syncing remote nodes done");
	
	for( unsigned int i = 0; i < bodies.size(); i++ )
	{
		Mesh* mesh = bodies[i]->getMeshes();
		LOG_DEBUG("Checking topology for mesh " << mesh->getId() );
		mesh->checkTopology(tau);
		LOG_DEBUG("Checking topology done");
		
		LOG_DEBUG("Looking for missed nodes");
		dataBus->syncMissedNodes(mesh, tau);
		LOG_DEBUG("Looking for missed nodes done");
		mesh->processCrackResponse();
	}
	
	// Run collision detector
	if( ! colDet->is_static() || currentTime == 0.0 )
	{
		colDet->find_collisions(virtNodes);
	}
	else
	{
		LOG_DEBUG("Collision detector call skipped since it is in static operation mode");
	}
}

void gcm::Engine::doNextStepStages(const float time_step)
{
	// FIXME - hardcoded name
	NumericalMethod *method = getNumericalMethod("InterpolationFixedAxis");
	for( int j = 0;  j < method->getNumberOfStages(); j++ )
	{
		LOG_DEBUG("Doing stage " << j);
		LOG_DEBUG("Syncing remote nodes");
		dataBus->syncNodes(time_step);
		LOG_DEBUG("Syncing remote nodes done");
		
		for( unsigned int i = 0; i < bodies.size(); i++ )
		{
			Mesh* mesh = bodies[i]->getMeshes();
			LOG_DEBUG( "Doing calculations for mesh " << mesh->getId() );
			mesh->do_next_part_step(time_step, j);
			LOG_DEBUG( "Mesh calculation done" );
		}
		LOG_DEBUG( "Stage done" );
	}
	LOG_DEBUG("Step done");
}

void gcm::Engine::doNextStepAfterStages(const float time_step) {
	for( unsigned int i = 0; i < bodies.size(); i++ )
	{
		RheologyCalculator* rc = getRheologyCalculator( bodies[i]->getRheologyCalculatorType() );
		Mesh* mesh = bodies[i]->getMeshes();
		LOG_DEBUG( "Applying rheology for mesh " << mesh->getId() );
		mesh->applyRheology(rc);
		LOG_DEBUG( "Applying rheology done" );
		LOG_DEBUG( "Processing stress state for mesh " << mesh->getId() );
		mesh->processStressState();
		LOG_DEBUG( "Processing stress state done" );
		mesh->processCrackState();
		//LOG_DEBUG( "Moving mesh " << mesh->getId() );
		//mesh->move_coords(time_step);
		//LOG_DEBUG( "Moving done" );
	}
	currentTime += time_step;
}

float gcm::Engine::getCurrentTime() {
	return currentTime;
}

void gcm::Engine::setCurrentTime(float time) {
	currentTime = time;
}

void gcm::Engine::syncNodes() {
	LOG_DEBUG("Syncing remote nodes");
	dataBus->syncNodes(-1);
	LOG_DEBUG("Syncing remote nodes done");
}

void gcm::Engine::syncOutlines() {
	LOG_DEBUG("Syncing outlines");
	dataBus->syncOutlines();
	LOG_DEBUG("Syncing outlines");
}

void gcm::Engine::calculate()
{
	// We set time step once and do not change it during calculation
	float tau = calculateRecommendedTimeStep();
	setTimeStep( tau );
	
	for( int i = 0; i < numberOfSnaps; i++ )
	{
		createSnapshot(i);
		for( int j = 0; j < stepsPerSnap; j++ )
			doNextStep();
	}
	
	createSnapshot(numberOfSnaps);
	createDump(numberOfSnaps);
}

float gcm::Engine::calculateRecommendedTimeStep()
{
	float timeStep = numeric_limits<float>::infinity();
	for( int j = 0; j < getNumberOfBodies(); j++ )
	{
		TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(j)->getMeshes();
		float tau = mesh->getRecommendedTimeStep();
		if( tau < timeStep )
			timeStep = tau;
	}
	return timeStep;
}

float gcm::Engine::calculateRecommendedContactTreshold(float tau)
{
	float threshold = numeric_limits<float>::infinity();
	// Threshold depends on mesh avg h
	if( contactThresholdType == CONTACT_THRESHOLD_BY_AVG_H )
	{
		for( int j = 0; j < getNumberOfBodies(); j++ )
		{
			TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(j)->getMeshes();
			float h = mesh->get_avg_h();
			if( h < threshold )
				threshold = h;
		}
	}
	// Threshold depends on max lambda * tau
	else if( contactThresholdType == CONTACT_THRESHOLD_BY_MAX_LT )
	{
		for( int j = 0; j < getNumberOfMaterials(); j++ )
		{
			Material* mat = getMaterial(j);
			float lt = tau * sqrt( ( mat->getLambda() + 2 * mat->getMu() ) / mat->getRho() );
			if( lt < threshold )
				threshold = lt;
		}
	}
	// Absolute threshold value
	else if( contactThresholdType == CONTACT_THRESHOLD_FIXED )
	{
		threshold = 1.0;
	}
	return threshold * contactThresholdFactor;
}

void gcm::Engine::createSnapshot(int number)
{
	for( int j = 0; j < getNumberOfBodies(); j++ )
	{
		TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(j)->getMeshes();
		if( mesh->getNumberOfLocalNodes() != 0 )
		{
			LOG_INFO( "Creating snapshot for mesh '" << mesh->getId() << "'" );
			vtkSnapshotWriter->dump(mesh, number);
		}
	}
}

void gcm::Engine::createDump(int number)
{
	for( int j = 0; j < getNumberOfBodies(); j++ )
	{
		TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(j)->getMeshes();
		if( mesh->getNodesNumber() != 0 )
		{
			LOG_INFO( "Creating dump for mesh '" << mesh->getId() << "'" );
			vtkDumpWriter->dump(mesh, number);
		}
	}
}

void gcm::Engine::setNumberOfSnaps(int number) {
	numberOfSnaps = number;
}

void gcm::Engine::setStepsPerSnap(int number) {
	stepsPerSnap = number;
}

DataBus* gcm::Engine::getDataBus() {
	return dataBus;
}

AABB gcm::Engine::getScene() {
	return scene;
}

void gcm::Engine::setScene(AABB src) {
	scene = src;
}

void gcm::Engine::transferScene(float x, float y, float z) {
	scene.transfer(x, y, z);
}

FileLookupService& gcm::Engine::getFileLookupService() {
	return fls;
}

void gcm::Engine::setContactThresholdType(unsigned char type)
{
	assert( type == CONTACT_THRESHOLD_BY_AVG_H 
			|| type == CONTACT_THRESHOLD_BY_MAX_LT 
			|| type == CONTACT_THRESHOLD_FIXED );
	contactThresholdType = type;
}

unsigned char gcm::Engine::getContactThresholdType()
{
	return contactThresholdType;
}

void gcm::Engine::setContactThresholdFactor(float val)
{
	assert( val > 0 );
	contactThresholdFactor = val;
}

float gcm::Engine::getContactThresholdFactor()
{
	return contactThresholdFactor;
}

void gcm::Engine::setDefaultRheologyCalculatorType(string calcType)
{
	defaultRheoCalcType = calcType;
}

string gcm::Engine::getDefaultRheologyCalculatorType()
{
	return defaultRheoCalcType;
}

void gcm::Engine::setCollisionDetectorStatic(bool val)
{
	colDet->set_static(val);
}

bool gcm::Engine::isCollisionDetectorStatic()
{
	return colDet->is_static();
}
