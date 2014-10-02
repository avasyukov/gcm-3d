#include "libgcm/Engine.hpp"

#include "libgcm/method/DummyMethod.hpp"
#include "libgcm/method/InterpolationFixedAxis.hpp"
#include "libgcm/calc/volume/SimpleVolumeCalculator.hpp"
//#include "libgcm/calc/border/ExternalVelocityCalculator.hpp"
#include "libgcm/calc/border/ExternalForceCalculator.hpp"
#include "libgcm/calc/border/FixedBorderCalculator.hpp"
#include "libgcm/calc/border/FreeBorderCalculator.hpp"
#include "libgcm/calc/border/SmoothBorderCalculator.hpp"
#include "libgcm/calc/contact/SlidingContactCalculator.hpp"
#include "libgcm/calc/contact/AdhesionContactCalculator.hpp"
#include "libgcm/calc/contact/AdhesionContactDestroyCalculator.hpp"
#include "libgcm/util/forms/StepPulseForm.hpp"
#include "libgcm/rheology/DummyRheologyCalculator.hpp"
#include "libgcm/BruteforceCollisionDetector.hpp"

using namespace gcm;
using std::string;
using std::vector;
using std::tuple;
using std::numeric_limits;
using std::function;

const std::string Engine::Options::SNAPSHOT_OUTPUT_PATH_PATTERN = "SNAPSHOT_OUTPUT_PATH_PATTERN";

Engine::Engine()
{
    rank = MPI::COMM_WORLD.Get_rank();
    numberOfWorkers = MPI::COMM_WORLD.Get_size();
    // get logger
    INIT_LOGGER("gcm.Engine");
    LOG_DEBUG("GCM engine created");
    LOG_DEBUG("Setting default engine values");
    LOG_DEBUG("Registering default methods");
    registerNumericalMethod( new DummyMethod() );
    registerNumericalMethod( new InterpolationFixedAxis() );
    LOG_DEBUG("Registering default interpolators");
    registerInterpolator( new TetrFirstOrderInterpolator() );
    registerInterpolator( new TetrSecondOrderMinMaxInterpolator() );
    registerInterpolator( new LineFirstOrderInterpolator() );
    LOG_DEBUG("Registering default rheology calculators");
    registerRheologyCalculator( new DummyRheologyCalculator() );
    defaultRheoCalcType = "DummyRheologyCalculator";
    LOG_DEBUG("Registering failure models");
    registerFailureModel( new NoFailureModel() );
    registerFailureModel( new CrackFailureModel() );
    registerFailureModel( new ScalarFailureModel() );
    registerFailureModel( new ContinualFailureModel() );
    registerFailureModel( new DebugFailureModel() );
    registerFailureModel( new HashinFailureModel() );
    defaultFailureModelType = "NoFailureModel";
    LOG_DEBUG("Registering default calculators");
    registerVolumeCalculator( new SimpleVolumeCalculator() );
    //registerBorderCalculator( new ExternalVelocityCalculator() );
    registerBorderCalculator( new ExternalForceCalculator() );
    registerBorderCalculator( new FixedBorderCalculator() );
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
    LOG_DEBUG("Creating data bus");
    dataBus = new DataBus();
    LOG_DEBUG("Creating collision detector");
    colDet = new BruteforceCollisionDetector();
    LOG_INFO("GCM engine initialized");
    currentTime = 0;
    currentTimeStep = 0;
    fixedTimeStep = -1;
    timeStepMultiplier = 1.0;
    numberOfSnaps = 0;
    stepsPerSnap = 1;
    contactThresholdType = CONTACT_THRESHOLD_BY_AVG_H;
    contactThresholdFactor = 1.0;
    meshesMovable = true;

    gmshVerbosity = 0.0;

    setOption(Options::SNAPSHOT_OUTPUT_PATH_PATTERN, "snap_mesh_%{MESH}_cpu_%{RANK}_step_%{STEP}.%{EXT}");
}

Engine::~Engine()
{
    cleanUp();
    LOG_INFO("GCM engine destroyed");
}

void Engine::clear() {
    // clear memory
    for(auto& b: bodies)
        delete b;
    bodies.clear();
    materials.clear();
    // clear state
    currentTime = 0;
    currentTimeStep = 0;
    fixedTimeStep = -1;
    timeStepMultiplier = 1.0;
    numberOfSnaps = 0;
    stepsPerSnap = 1;
    contactThresholdType = CONTACT_THRESHOLD_BY_AVG_H;
    contactThresholdFactor = 1.0;
    snapshots.clear();
}

void Engine::cleanUp()
{
    clear();
    delete dataBus;
    //delete vtkSnapshotWriter;
    //delete vtkDumpWriter;
    delete colDet;
    LOG_INFO("Clean up done");
}

int Engine::getRank()
{
    return rank;
}

int Engine::getNumberOfWorkers()
{
    return numberOfWorkers;
}

void Engine::setTimeStep(float dt)
{
    if(dt > 0)
        fixedTimeStep = dt;
}

float Engine::getTimeStep()
{
    return fixedTimeStep;
}

void Engine::setTimeStepMultiplier(float m)
{
    if(m > 0)
        timeStepMultiplier = m;
}

float Engine::getTimeStepMultiplier()
{
    return timeStepMultiplier;
}

GCMDispatcher* Engine::getDispatcher()
{
    return dispatcher;
}

void Engine::registerVolumeCalculator(VolumeCalculator *volumeCalculator)
{
    if (!volumeCalculator)
        THROW_INVALID_ARG("Volume calculator parameter cannot be NULL");
    volumeCalculators[volumeCalculator->getType()] = volumeCalculator;
    LOG_DEBUG("Registered volume calculator: " << volumeCalculator->getType());
}

void Engine::registerBorderCalculator(BorderCalculator *borderCalculator)
{
    if (!borderCalculator)
        THROW_INVALID_ARG("Border calculator parameter cannot be NULL");
    borderCalculators[borderCalculator->getType()] = borderCalculator;
    LOG_DEBUG("Registered border calculator: " << borderCalculator->getType());
}

void Engine::registerContactCalculator(ContactCalculator *contactCalculator)
{
    if (!contactCalculator)
        THROW_INVALID_ARG("Contact calculator parameter cannot be NULL");
    contactCalculators[contactCalculator->getType()] = contactCalculator;
    LOG_DEBUG("Registered contact calculator: " << contactCalculator->getType());
}

void Engine::registerNumericalMethod(NumericalMethod *numericalMethod)
{
    if (!numericalMethod)
        THROW_INVALID_ARG("Numerical method parameter cannot be NULL");
    numericalMethods[numericalMethod->getType()] = numericalMethod;
    LOG_DEBUG("Registered numerical method: " << numericalMethod->getType());
}

void Engine::registerInterpolator(TetrInterpolator* interpolator)
{
    if (!interpolator)
        THROW_INVALID_ARG("Interpolator parameter cannot be NULL");
    interpolators[interpolator->getType()] = interpolator;
    LOG_DEBUG("Registered interpolator: " << interpolator->getType());
}

void Engine::registerRheologyCalculator(RheologyCalculator* rheologyCalculator)
{
    if (!rheologyCalculator)
        THROW_INVALID_ARG("Rheology calculator parameter cannot be NULL");
    rheologyCalculators[rheologyCalculator->getType()] = rheologyCalculator;
    LOG_DEBUG("Registered rheology calculator: " << rheologyCalculator->getType());
}

void Engine:: registerFailureModel(FailureModel *model)
{
    if (!model)
        THROW_INVALID_ARG("Failure model parameter cannot be NULL");
    failureModels[model->getType()] = model;
    LOG_DEBUG("Registered failure model: " << model->getType());
}

unsigned char Engine::addMaterial(MaterialPtr material)
{
    if( !material )
        THROW_INVALID_ARG("Material parameter cannot be NULL");
    materials.push_back(material);
    unsigned char index = materials.size() - 1;
    LOG_DEBUG("Added new material. Name: " << material->getName() << " Index: " << (int)index);
    return index;
}

unsigned int Engine::addBorderCondition(BorderCondition *borderCondition)
{
    if (!borderCondition)
        THROW_INVALID_ARG("Border condition parameter cannot be NULL");
    borderConditions.push_back(borderCondition);
    LOG_DEBUG("Added new border condition.");
    return borderConditions.size () - 1;
}

void Engine::replaceDefaultBorderCondition(BorderCondition *borderCondition)
{
    assert_gt(borderConditions.size(), 1 );
    if (!borderCondition)
        THROW_INVALID_ARG("Border condition parameter cannot be NULL");
    borderConditions[1] = borderCondition;
    LOG_DEBUG("Default border condition set");
}

unsigned int Engine::addContactCondition(ContactCondition *contactCondition)
{
    if (!contactCondition)
        THROW_INVALID_ARG("Contact condition parameter cannot be NULL");
    contactConditions.push_back(contactCondition);
    LOG_DEBUG("Added new contact condition.");
    return contactConditions.size () - 1;
}

void Engine::replaceDefaultContactCondition(ContactCondition *contactCondition)
{
    assert_gt(contactConditions.size(), 0 );
    if (!contactCondition)
        THROW_INVALID_ARG("Contact condition parameter cannot be NULL");
    contactConditions[0] = contactCondition;
    LOG_DEBUG("Default contact condition set");
}

unsigned char Engine::getMaterialIndex(string name)
{
    for (unsigned char i = 0; i < materials.size(); i++)
        if (materials[i]->getName() == name)
            return i;
    THROW_INVALID_ARG("Material was not found");
}

const MaterialPtr& Engine::getMaterial(string name)
{
    for (unsigned char i = 0; i < materials.size(); i++)
        if (materials[i]->getName() == name)
            return materials[i];
    THROW_INVALID_ARG("Material was not found");
}

const MaterialPtr& Engine::getMaterial(unsigned char index)
{
    assert_ge(index, 0);
    assert_lt(index, materials.size());
    return materials[index];
}

Body* Engine::getBodyById(string id)
{
    for (size_t i = 0; i < bodies.size(); i++)
        if (bodies[i]->getId() == id)
            return bodies[i];
    LOG_WARN("Body with id '" << id << "' was not found");
    return NULL;
}

int Engine::getBodyNum(string id)
{
    for (size_t i = 0; i < bodies.size(); i++)
        if (bodies[i]->getId() == id)
            return i;
    LOG_WARN("Body with id '" << id << "' was not found");
    return -1;
}

Body* Engine::getBody(unsigned int num)
{
    assert_lt(num, bodies.size() );
    return bodies[num];
}

int Engine::getNumberOfBodies()
{
    return bodies.size();
}

int Engine::getNumberOfMaterials()
{
    return materials.size();
}

NumericalMethod* Engine::getNumericalMethod(string type)
{
    return numericalMethods.find(type) != numericalMethods.end() ? numericalMethods[type] : NULL;
}

VolumeCalculator* Engine::getVolumeCalculator(string type)
{
    return volumeCalculators.find(type) != volumeCalculators.end() ? volumeCalculators[type] : NULL;
}

BorderCalculator* Engine::getBorderCalculator(string type)
{
    return borderCalculators.find(type) != borderCalculators.end() ? borderCalculators[type] : NULL;
}

ContactCalculator* Engine::getContactCalculator(string type)
{
    return contactCalculators.find(type) != contactCalculators.end() ? contactCalculators[type] : NULL;
}

LineFirstOrderInterpolator* Engine::getFirstOrderLineInterpolator(string type)
{
    return interpolators.find(type) != interpolators.end() ? (LineFirstOrderInterpolator*) interpolators[type] : NULL;
}

TetrFirstOrderInterpolator* Engine::getFirstOrderInterpolator(string type)
{
    return interpolators.find(type) != interpolators.end() ? (TetrFirstOrderInterpolator*) interpolators[type] : NULL;
}

TetrSecondOrderMinMaxInterpolator* Engine::getSecondOrderInterpolator(string type)
{
    return interpolators.find(type) != interpolators.end() ? (TetrSecondOrderMinMaxInterpolator*) interpolators[type] : NULL;
}

RheologyCalculator* Engine::getRheologyCalculator(string type)
{
    return rheologyCalculators.find(type) != rheologyCalculators.end() ? rheologyCalculators[type] : NULL;
}

FailureModel* Engine::getFailureModel(string type)
{
    return failureModels.find(type) != failureModels.end() ? failureModels[type] : NULL;
}

BorderCondition* Engine::getBorderCondition(unsigned int num)
{
    assert_lt(num, borderConditions.size() );
    return borderConditions[num];
}

ContactCondition* Engine::getContactCondition(unsigned int num)
{
    assert_lt(num, contactConditions.size() );
    return contactConditions[num];
}

void Engine::addBody(Body* body)
{
    bodies.push_back(body);
}

CalcNode& Engine::getVirtNode(unsigned int i)
{
    assert_ge(i, 0);
    assert_lt(i, virtNodes.size());
    return virtNodes[i];
}

void Engine::doNextStep()
{
    float step;
    doNextStepBeforeStages(numeric_limits<float>::infinity(), step);
    doNextStepStages(step);
    doNextStepAfterStages(step);
    currentTimeStep++;
}

void Engine::doNextStepBeforeStages(const float maxAllowedStep, float& actualTimeStep)
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
        tau = timeStepMultiplier * calculateRecommendedTimeStep();
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

        //LOG_DEBUG("Processing response from cracks");
        //mesh->processCrackResponse();
        //LOG_DEBUG("Processing response from cracks done");
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

void Engine::doNextStepStages(const float time_step)
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
            mesh->doNextPartStep(time_step, j);
            LOG_DEBUG( "Mesh calculation done" );
            LOG_DEBUG( "Applying correctors for mesh " << mesh->getId() );
            mesh->applyCorrectors();
            LOG_DEBUG( "Applying correctors done" );
        }
        LOG_DEBUG( "Stage done" );
    }
    LOG_DEBUG("Step done");
}

void Engine::doNextStepAfterStages(const float time_step) {
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
        string failureType = getDefaultFailureModelType();
        LOG_DEBUG( "Processing failure for mesh " << mesh->getId() << " using " << failureType << " model" );
        mesh->processMaterialFailure( getFailureModel(failureType), time_step );
        LOG_DEBUG( "Processing failure done" );
        if( getMeshesMovable() && mesh->getMovable() )
        {
            LOG_DEBUG( "Moving mesh " << mesh->getId() );
            mesh->moveCoords(time_step);
            LOG_DEBUG( "Moving done" );
        }
        else
        {
            LOG_DEBUG( "Not moving mesh " << mesh->getId() );
        }
    }
    currentTime += time_step;
}

float Engine::getCurrentTime() {
    return currentTime;
}

void Engine::setCurrentTime(float time) {
    currentTime = time;
}

void Engine::syncNodes() {
    LOG_DEBUG("Syncing remote nodes");
    dataBus->syncNodes(-1);
    LOG_DEBUG("Syncing remote nodes done");
}

void Engine::syncOutlines() {
    LOG_DEBUG("Syncing outlines");
    dataBus->syncOutlines();
    LOG_DEBUG("Syncing outlines");
}

void Engine::calculate()
{
    // We set time step once and do not change it during calculation
    // float tau = calculateRecommendedTimeStep();
    // setTimeStep( tau );

    for( int i = 0; i < numberOfSnaps; i++ )
    {
        snapshotTimestamps.push_back(getCurrentTime());
        createSnapshot(i);
        for( int j = 0; j < stepsPerSnap; j++ )
            doNextStep();
    }

    snapshotTimestamps.push_back(getCurrentTime());
    createSnapshot(numberOfSnaps);
    createDump(numberOfSnaps);
}

float Engine::calculateRecommendedTimeStep()
{
    float timeStep = numeric_limits<float>::infinity();
    for( int j = 0; j < getNumberOfBodies(); j++ )
    {
        float tau = getBody(j)->getMeshes()->getRecommendedTimeStep();
        if( tau < timeStep )
            timeStep = tau;
    }
    return timeStep;
}

float Engine::calculateRecommendedContactTreshold(float tau)
{
    float threshold = numeric_limits<float>::infinity();
    // Threshold depends on mesh avg h
    if( contactThresholdType == CONTACT_THRESHOLD_BY_AVG_H )
    {
        for( int j = 0; j < getNumberOfBodies(); j++ )
        {
            TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(j)->getMeshes();
            float h = mesh->getAvgH();
            if( h < threshold )
                threshold = h;
        }
    }
    // Threshold depends on max lambda * tau
    else if( contactThresholdType == CONTACT_THRESHOLD_BY_MAX_LT )
    {
            THROW_UNSUPPORTED("Dynamic contact threshold computation not supported yet");
//        for( int j = 0; j < getNumberOfMaterials(); j++ )
//        {
//            Material* mat = getMaterial(j);
//            float lt = tau * sqrt( ( mat->getLambda() + 2 * mat->getMu() ) / mat->getRho() );
//            if( lt < threshold )
//                threshold = lt;
//        }
    }
    // Absolute threshold value
    else if( contactThresholdType == CONTACT_THRESHOLD_FIXED )
    {
        threshold = 1.0;
    }
    return threshold * contactThresholdFactor;
}

void Engine::createSnapshot(int number)
{
    for( int j = 0; j < getNumberOfBodies(); j++ )
    {
        Mesh* mesh = getBody(j)->getMeshes();
        if( mesh->getNumberOfLocalNodes() != 0 )
        {
            LOG_INFO( "Creating snapshot for mesh '" << mesh->getId() << "'" );
            auto snapName = mesh->snapshot(number);
            snapshots.push_back(make_tuple(number, mesh->getId(), snapName));
        }
    }
}

void Engine::createDump(int number)
{
    for( int j = 0; j < getNumberOfBodies(); j++ )
    {
        TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(j)->getMeshes();
        if( mesh->getNodesNumber() != 0 )
        {
            LOG_INFO( "Creating dump for mesh '" << mesh->getId() << "'" );
            mesh->dump(number);
        }
    }
}

void Engine::setNumberOfSnaps(int number) {
    numberOfSnaps = number;
}

void Engine::setStepsPerSnap(int number) {
    stepsPerSnap = number;
}

DataBus* Engine::getDataBus() {
    return dataBus;
}

AABB Engine::getScene() {
    return scene;
}

void Engine::setScene(AABB src) {
    scene = src;
}

void Engine::transferScene(float x, float y, float z) {
    scene.transfer(x, y, z);
}

void Engine::scaleScene(float x0, float y0, float z0, 
		float scaleX, float scaleY, float scaleZ) {
    scene.scale(x0, y0, z0, scaleX, scaleY, scaleZ);
}

void Engine::setContactThresholdType(unsigned char type)
{
    assert_true( type == CONTACT_THRESHOLD_BY_AVG_H
            || type == CONTACT_THRESHOLD_BY_MAX_LT
            || type == CONTACT_THRESHOLD_FIXED );
    contactThresholdType = type;
}

unsigned char Engine::getContactThresholdType()
{
    return contactThresholdType;
}

void Engine::setContactThresholdFactor(float val)
{
    assert_gt(val, 0 );
    contactThresholdFactor = val;
}

float Engine::getContactThresholdFactor()
{
    return contactThresholdFactor;
}

void Engine::setDefaultRheologyCalculatorType(string calcType)
{
    defaultRheoCalcType = calcType;
}

string Engine::getDefaultRheologyCalculatorType()
{
    return defaultRheoCalcType;
}

void Engine::setDefaultFailureModelType(string modelType)
{
    defaultFailureModelType = modelType;
}

string Engine::getDefaultFailureModelType()
{
    return defaultFailureModelType;
}

void Engine::setCollisionDetectorStatic(bool val)
{
    colDet->set_static(val);
}

bool Engine::isCollisionDetectorStatic()
{
    return colDet->is_static();
}

void Engine::setMeshesMovable(bool val)
{
    meshesMovable = val;
}

bool Engine::getMeshesMovable()
{
    return meshesMovable;
}

float Engine::getGmshVerbosity() {
    return gmshVerbosity;
}

void Engine::setGmshVerbosity(float verbosity) {
    gmshVerbosity = verbosity;
}

bool Engine::interpolateNode(CalcNode& node)
{
    for( unsigned int i = 0; i < bodies.size(); i++ )
    {
        Mesh* mesh = bodies[i]->getMeshes();
        if( mesh->interpolateNode(node) )
            return true;
    }
    return false;
}
        
void Engine::setRheologyMatrices(function<RheologyMatrixPtr (const CalcNode&)> getMatrixForNode)
{
    for (auto& b: bodies)
        for (auto& m: b->getMeshesVector())
            for (int i = 0; i < m->getNodesNumber(); i++)
            {
                CalcNode& node = m->getNodeByLocalIndex(i);
                if (node.isUsed() || node.isLocal(false))
                    node.setRheologyMatrix(getMatrixForNode(node));
            }
}

const vector<tuple<unsigned int, string, string>>& Engine::getSnapshotsList() const
{
    return snapshots;
}

const vector<float>& Engine::getSnapshotTimestamps() const
{
	return snapshotTimestamps;
}

void Engine::setOption(string option, string value)
{
    options[option] = value;
}

const string& Engine::getOption(string option) const
{
    if (!hasOption(option))
        THROW_INVALID_ARG("Option \"" + option + "\" not found");
    return options.at(option);
}

bool Engine::hasOption(string option) const
{
    return options.find(option) != options.end();
}
