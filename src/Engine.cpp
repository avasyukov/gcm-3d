#include "Engine.h"

// initialiaze static fields
int gcm::Engine::enginesNumber = 0;

gcm::Engine::Engine()
{
	// check engines counter
	if (enginesNumber)
	{
		// because of some internal architecture limitations we support
		// only one instance of engine at the same time
		throw logic_error("Only one engine can be used at the same time");
	}
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
	LOG_DEBUG("Registering default node factories");
	registerNodeFactory(new ElasticNodeFactory());
	LOG_DEBUG("Registering default methods");
	registerNumericalMethod( new InterpolationFixedAxis() );
	LOG_DEBUG("Registering default interpolators");
	registerInterpolator( new TetrFirstOrderInterpolator() );
	registerInterpolator( new TetrSecondOrderMinMaxInterpolator() );
	LOG_DEBUG("Registering default rheology calculators");
	registerRheologyCalculator( new RheologyCalculator() );
	LOG_DEBUG("Registering default calculators");
	registerVolumeCalculator( new SimpleVolumeCalculator() );
	registerBorderCalculator( new FreeBorderCalculator() );
	registerBorderCalculator( new SmoothBorderCalculator() );
	LOG_DEBUG("Registering default border condition");
	addBorderCondition( new BorderCondition( NULL, new StepPulseForm(-1, -1), getBorderCalculator("SmoothBorderCalculator") ) );
	addBorderCondition( new BorderCondition( NULL, new StepPulseForm(-1, -1), getBorderCalculator("FreeBorderCalculator") ) );
	LOG_DEBUG("Creating dispatcher");
	dispatcher = new DummyDispatcher();
	dispatcher->setEngine(this);
	LOG_DEBUG("Creating data bus");
	dataBus = new DataBus();
	dataBus->setEngine(this);
	LOG_INFO("GCM engine initialized");
	currentTime = 0;
	fixedTimeStep = -1;
	numberOfSnaps = 0;
	stepsPerSnap = 1;
}

gcm::Engine::Engine(int* argc, char*** argv)
{
	// TODO process arguments
	// call default constructor
	Engine();
}

gcm::Engine::~Engine()
{
	// clear memory
	foreach(b, bodies)
		delete *b;
	// decrement engines counter
	enginesNumber--;
	delete dataBus;
	// shutdown MPI
	if( !MPI::Is_finalized() )
		MPI::Finalize();
	LOG_INFO("GCM engine destroyed");
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

void gcm::Engine::loadSceneFromFile(string fileName)
{
	// FIXME shoul we validate task file against xml schema?
	LOG_INFO("Loading scene from file " << fileName);
	// parse file
	xmlpp::DomParser parser;
	parser.parse_file(fileName);
	Element* rootElement = parser.get_document()->get_root_node();
	// search for bodies
	NodeSet bodyNodes = rootElement->find("/task/bodies/body");
	foreach(bodyNode, bodyNodes)
	{
		Element* bodyEl = dynamic_cast<Element*>(*bodyNode);
		string id = bodyEl->get_attribute_value("id");
		LOG_DEBUG("Loading body '" << id << "'");
		// create body instance
		Body* body = new Body(id);
		body->setEngine(this);
		// set rheology
		xmlpp::Node::NodeList rheologyNodes = bodyEl->get_children("rheology");
		if (rheologyNodes.size() > 1)
			THROW_INVALID_INPUT("Only one rheology element allowed for body declaration");
		if (rheologyNodes.size()) {
			Element *rheologyEl = dynamic_cast<Element*>(rheologyNodes.front());
			body->setRheology(getAttributeByName(rheologyEl->get_attributes(), "type"));
		}

		// load meshes
		xmlpp::Node::NodeList meshNodes = bodyEl->get_children("mesh");
		foreach(meshNode, meshNodes)
		{
			Element* meshEl = static_cast<Element*>(*meshNode);
			// get mesh parameters
			Params params(meshEl);
			if (!params.has("type"))
			{
				// FIXME should we throw exception here?
				LOG_WARN("Mesh type is not specified, mesh will be ignored.");
				break;
			}
			MeshLoader* meshLoader = getMeshLoader(params["type"]);
			if (!meshLoader)
			{
				// FIXME should we throw exception?
				LOG_WARN("Mesh loader for type '" << params["type"] << "' " <<
						"not found, mesh will be ignored.");
				break;
			}
			
			LOG_INFO("Loading mesh");
			// TODO - think about multiple bodies and multiple meshes per body
			meshLoader->preLoadMesh(params, &scene);
			LOG_DEBUG("Mesh preloaded. Scene size: " << scene );

			dispatcher->prepare(numberOfWorkers, &scene);
			dataBus->syncOutlines();
			for( int i = 0; i < numberOfWorkers; i++)
			{
				LOG_DEBUG("Area scheduled for worker " << i << ": " << *(dispatcher->getOutline(i)));
			}	
			
			// use loader to load mesh
			Mesh* mesh = meshLoader->load(body, params);
			// attach mesh to body
			body->attachMesh(mesh);
			LOG_INFO("Mesh '" << mesh->getId() << "' of type '" <<  meshLoader->getType() << "' created");
		}
		// add body to scene
		addBody(body);
		LOG_DEBUG("Body '" << id << "' loaded");
	}	
	// FIXME - rewrite this indian style code
	NodeSet initialStateNodes = rootElement->find("/task/initialState");
	foreach(initialStateNode, initialStateNodes)
	{
		Element* stateEl = dynamic_cast<Element*>(*initialStateNode);
		Area* stateArea;
		float values[9];
		xmlpp::Node::NodeList areaNodes = stateEl->get_children("area");
		if (areaNodes.size() > 1)
			THROW_INVALID_INPUT("Only one area element allowed for initial state");
		if (areaNodes.size()) {
			Element *areaEl = dynamic_cast<Element*>(areaNodes.front());
			string areaType = getAttributeByName(areaEl->get_attributes(), "type");
			if( areaType == "box" )
			{
				LOG_DEBUG("Initial state area: " << areaType);
				float minX = atof( getAttributeByName(areaEl->get_attributes(), "minX").c_str() );
				float maxX = atof( getAttributeByName(areaEl->get_attributes(), "maxX").c_str() );
				float minY = atof( getAttributeByName(areaEl->get_attributes(), "minY").c_str() );
				float maxY = atof( getAttributeByName(areaEl->get_attributes(), "maxY").c_str() );
				float minZ = atof( getAttributeByName(areaEl->get_attributes(), "minZ").c_str() );
				float maxZ = atof( getAttributeByName(areaEl->get_attributes(), "maxZ").c_str() );
				LOG_DEBUG("Box size: [" << minX << ", " << maxX << "] " 
									<< "[" << minY << ", " << maxY << "] " 
									<< "[" << minZ << ", " << maxZ << "]");
				stateArea = new BoxArea(minX, maxX, minY, maxY, minZ, maxZ);
			} else {
				LOG_WARN("Unknown initial state area: " << areaType);
			}
		}
		xmlpp::Node::NodeList valuesNodes = stateEl->get_children("values");
		if (valuesNodes.size() > 1)
			THROW_INVALID_INPUT("Only one values element allowed for initial state");
		if (valuesNodes.size()) {
			memset(values, 0, 9*sizeof(float));
			Element *valuesEl = dynamic_cast<Element*>(valuesNodes.front());
			string vx = valuesEl->get_attribute_value("vx");
			if( !vx.empty() )
				values[0] = atof( vx.c_str() );
			string vy = valuesEl->get_attribute_value("vy");
			if( !vy.empty() )
				values[1] = atof( vy.c_str() );
			string vz = valuesEl->get_attribute_value("vz");
			if( !vz.empty() )
				values[2] = atof( vz.c_str() );
			string sxx = valuesEl->get_attribute_value("sxx");
			if( !sxx.empty() )
				values[3] = atof( sxx.c_str() );
			string sxy = valuesEl->get_attribute_value("sxy");
			if( !sxy.empty() )
				values[4] = atof( sxy.c_str() );
			string sxz = valuesEl->get_attribute_value("sxz");
			if( !sxz.empty() )
				values[5] = atof( sxz.c_str() );
			string syy = valuesEl->get_attribute_value("syy");
			if( !syy.empty() )
				values[6] = atof( syy.c_str() );
			string syz = valuesEl->get_attribute_value("syz");
			if( !syz.empty() )
				values[7] = atof( syz.c_str() );
			string szz = valuesEl->get_attribute_value("szz");
			if( !szz.empty() )
				values[8] = atof( szz.c_str() );
			LOG_DEBUG("Initial state values: " 
							<< values[0] << " " << values[1] << " " << values[2] << " " 
							<< values[3] << " " << values[4] << " " << values[5] << " "
							<< values[6] << " " << values[7] << " " << values[8] );
		}
		for( unsigned int i = 0; i < bodies.size(); i++ )
			bodies[i]->setInitialState(stateArea, values);
	}
	LOG_DEBUG("Scene loaded");
}

void gcm::Engine::registerSnapshotWriter(SnapshotWriter* snapshotWriter)
{
	if (!snapshotWriter)
		THROW_INVALID_ARG("Snapshot writer parameter cannot be NULL");
	snapshotWriters[snapshotWriter->getType()] = snapshotWriter;
	LOG_DEBUG("Registered snapshot writer: " << snapshotWriter->getType());
}

void gcm::Engine::registerNodeFactory(NodeFactory* nodeFactory)
{
	if (!nodeFactory)
		THROW_INVALID_ARG("Snapshot writer parameter cannot be NULL");
	nodeFactories[nodeFactory->getType()] = nodeFactory;
	LOG_DEBUG("Registered node factory: " << nodeFactory->getType());
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

void gcm::Engine::addBorderCondition(BorderCondition *borderCondition)
{
	if (!borderCondition)
		THROW_INVALID_ARG("Border condition parameter cannot be NULL");
	borderConditions.push_back(borderCondition);
	LOG_DEBUG("Added new border condition.");
}

Body* gcm::Engine::getBodyById(string id)
{
	for (size_t i = 0; i < bodies.size(); i++)
		if (bodies[i]->getId() == id)
			return bodies[i];
	LOG_WARN("Body with id '" << id << "' was not found");
	return NULL;
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

MeshLoader* gcm::Engine::getMeshLoader(string type)
{
	return meshLoaders.find(type) != meshLoaders.end() ? meshLoaders[type] : NULL;
}

SnapshotWriter* gcm::Engine::getSnapshotWriter(string type)
{
	return snapshotWriters.find(type) != snapshotWriters.end() ? snapshotWriters[type] : NULL;
}

NodeFactory* gcm::Engine::getNodeFactory(string type)
{
	return nodeFactories.find(type) != nodeFactories.end() ? nodeFactories[type] : NULL;
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

void gcm::Engine::addBody(Body* body)
{
	bodies.push_back(body);
}

void gcm::Engine::doNextStep()
{
	float step;
	doNextStepStages(numeric_limits<float>::infinity(), step);
	doNextStepAfterStages(step);
}

void gcm::Engine::doNextStepStages(const float maxAllowedStep, float& actualTimeStep)
{	
	LOG_INFO("Starting next step, current time is " << currentTime);
	
	LOG_DEBUG("Space zones for parallel calculations:");
	dispatcher->printZones();
	
	NumericalMethod *method = getNumericalMethod("InterpolationFixedAxis");
	LOG_DEBUG( "Number of stages: " << method->getNumberOfStages() );
	
	float tau = numeric_limits<float>::infinity();
	
	if( fixedTimeStep > 0 ) {
		tau = fixedTimeStep;
	} else {
		for( unsigned int i = 0; i < bodies.size(); i++ )
		{
			Mesh* mesh = bodies[i]->getMeshes();
			float t = mesh->getRecommendedTimeStep();
			if( t < tau )
				tau = t;
		}
	}
	LOG_DEBUG( "Local time step " << tau );
	if (tau > maxAllowedStep) tau = maxAllowedStep;
	dataBus->syncTimeStep(&tau);
	LOG_INFO("Time step synchronized, value is: " << tau);
	
	for( int j = 0;  j < method->getNumberOfStages(); j++ )
	{
		LOG_INFO( "Doing stage " << j );
		LOG_DEBUG("Syncing outlines");
		dataBus->syncOutlines();
		LOG_DEBUG("Syncing outlines done");
		LOG_DEBUG("Syncing remote nodes");
		dataBus->syncNodes(tau);
		LOG_DEBUG("Syncing remote nodes done");
		for( unsigned int i = 0; i < bodies.size(); i++ )
		{
			LOG_DEBUG("Checking topology for mesh " << i );
			Mesh* mesh = bodies[i]->getMeshes();
			mesh->checkTopology(tau);
			LOG_DEBUG("Checking topology done");
			LOG_DEBUG("Looking for missed nodes");
			dataBus->syncMissedNodes(tau);
			LOG_DEBUG("Looking for missed nodes done");
		}
		for( unsigned int i = 0; i < bodies.size(); i++ )
		{
			LOG_DEBUG( "Doing calculations for mesh " << i );
			Mesh* mesh = bodies[i]->getMeshes();
			mesh->do_next_part_step(tau, j);
			LOG_DEBUG( "Mesh calculation done" );
		}
		LOG_DEBUG( "Stage done" );
	}
	LOG_INFO("Step done");
	actualTimeStep = tau;
}

void gcm::Engine::doNextStepAfterStages(const float time_step) {
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
	VTKSnapshotWriter* sw = new VTKSnapshotWriter();
	TetrMeshSecondOrder* mesh = (TetrMeshSecondOrder*)getBody(0)->getMeshes();
	
	setTimeStep( mesh->getRecommendedTimeStep() );
	//engine->setTimeStep( 2 * mesh->getMaxPossibleTimeStep() );
		
	for( int i = 0; i < numberOfSnaps; i++ )
	{
		LOG_INFO( "Dumping mesh " << mesh->getId() );
		//syncNodes();
		sw->dump(mesh, i);
		
		for( int j = 0; j < stepsPerSnap; j++ )
			doNextStep();
	}
		
	LOG_INFO( "Dumping mesh " << mesh->getId() );
	//syncNodes();
	sw->dump(mesh, numberOfSnaps);
	
	delete sw;
}

void gcm::Engine::setNumberOfSnaps(int number) {
	numberOfSnaps = number;
}

void gcm::Engine::setStepsPerSnap(int number) {
	stepsPerSnap = number;
}