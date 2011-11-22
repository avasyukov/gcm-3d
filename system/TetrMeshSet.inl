TetrMeshSet::TetrMeshSet()
{
	logger = NULL;
	data_bus = NULL;
	stresser = NULL;
	rheology = NULL;
	numerical_method = NULL;
	collision_detector = NULL;
};

TetrMeshSet::~TetrMeshSet() { };

void TetrMeshSet::attach(Logger* new_logger)
{
	logger = new_logger;

	for(int i = 0; i < meshes.size(); i++)
		meshes[i]->attach(logger);
};

void TetrMeshSet::attach(TetrMesh_1stOrder* new_mesh)
{
	if(new_mesh != NULL) {
		new_mesh->attach(logger);
		new_mesh->attach(data_bus);
		new_mesh->attach(stresser);
		new_mesh->attach(rheology);
		new_mesh->attach(numerical_method);
		meshes.push_back(new_mesh);
	}
};

void TetrMeshSet::attach(DataBus* new_data_bus)
{
	data_bus = new_data_bus;

	for(int i = 0; i < meshes.size(); i++)
		meshes[i]->attach(new_data_bus);
};

void TetrMeshSet::attach(Stresser* new_stresser)
{
	stresser = new_stresser;

	for(int i = 0; i < meshes.size(); i++)
		meshes[i]->attach(new_stresser);
};

void TetrMeshSet::attach(RheologyCalculator* new_rheology)
{
	rheology = new_rheology;

	for(int i = 0; i < meshes.size(); i++)
		meshes[i]->attach(new_rheology);
};

void TetrMeshSet::attach(TetrNumericalMethod* new_numerical_method)
{
	numerical_method = new_numerical_method;

	for(int i = 0; i < meshes.size(); i++)
		meshes[i]->attach(new_numerical_method);
};

void TetrMeshSet::attach(CollisionDetector* new_collision_detector)
{
	collision_detector = new_collision_detector;
};

void TetrMeshSet::log_meshes_types()
{
	if(logger != NULL)
		for(int i = 0; i < meshes.size(); i++) {
			stringstream ss;
			ss << "Mesh #" << i << ". Type: ";
			logger->write(ss.str() + *(meshes[i]->get_mesh_type()));
		}
};

void TetrMeshSet::log_meshes_stats()
{
	if(logger != NULL)
		for(int i = 0; i < meshes.size(); i++) {
			stringstream ss;
			ss << "Mesh #" << i << ". Stats: ";
			logger->write(ss.str());
			meshes[i]->log_mesh_stats();
		}
};

float TetrMeshSet::get_current_time()
{
	float time;
	if(meshes.size() == 0)
		return -1;

	time = meshes[0]->get_current_time();

	for(int i = 0; i < meshes.size(); i++) {
		if( fabs(time - meshes[i]->get_current_time()) > 0.001 * time ) {
			if(logger != NULL)
				logger->write("ERROR: meshes report different time!");
			return -1;
		}
	}

	return time;
};

int TetrMeshSet::do_next_step()
{
	for(int i = 0; i < meshes.size(); i++)
		for(int j = i+1; j < meshes.size(); j++)
			if( collision_detector->find_collisions(meshes[i], meshes[j]) < 0 )
				return -1;

	for(int i = 0; i < meshes.size(); i++)
		if (meshes[i]->do_next_step() < 0)
			return -1;
	return 0;
};

int TetrMeshSet::get_number_of_meshes()
{
	return meshes.size();
};

TetrMesh_1stOrder* TetrMeshSet::get_mesh(int num)
{
	if(num < 0 || num >= meshes.size())
		return NULL;
	return meshes[num];
};
