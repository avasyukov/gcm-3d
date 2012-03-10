#include "TetrMeshSet.h"

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
		new_mesh->attach_data_bus(data_bus);
		new_mesh->attach(stresser);
		new_mesh->attach(rheology);
		// FIXME - never ever do it! We need separate copy of method class for each mesh - see below.
		// new_mesh->attach(numerical_method);
		new_mesh->attach(this);
		meshes.push_back(new_mesh);
//		// process mesh and add all remote nodes to list of nodes to be synchronized
//		for (int i = 0; i < new_mesh->nodes.size(); i++) {
//			ElasticNode *node = &new_mesh->nodes[i];
//			if (node->placement_type == REMOTE)
//				remote_nodes[data_bus->get_proc_for_zone(node->zone_num)].push_back(node);
//		}
//	TODO process remote nodes
	}
};

void TetrMeshSet::attach(DataBus* new_data_bus)
{
	int i;
	
	data_bus = new_data_bus;

	data_bus->attach(this);

	for(int i = 0; i < meshes.size(); i++)
		meshes[i]->attach_data_bus(new_data_bus);

//	// initialize remote nodes store
//	FIXME
//	for (i = 0; i < data_bus->get_total_proc_num(); i++)
//		remote_nodes.push_back(vector<ElasticNode*>());

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

	// FIXME - never ever do it! We need separate copy of method class for each mesh 
	// because it stores local vars like random basis data.
	// for(int i = 0; i < meshes.size(); i++)
	//	meshes[i]->attach(new_numerical_method);
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
		if( fabs(time - meshes[i]->get_current_time()) > 0.001 * time ) { // TODO - remove magic number
			if(logger != NULL)
				logger->write("ERROR: TetrMeshSet - meshes report different time!");
			return -1;
		}
	}

	return time;
};

int TetrMeshSet::do_next_step()
{

//	float time_step = meshes[0]->get_max_possible_tau();
//	for(int i = 1; i < meshes.size(); i++) {
//		float ftmp = meshes[i]->get_max_possible_tau();
//		if(ftmp < time_step)
//			time_step = ftmp;
//	}
	float time_step = data_bus->get_max_possible_tau(get_max_possible_tau());

	// Clear virtual nodes because they change between time steps
	virt_nodes.clear();
	for(int i = 0; i < meshes.size(); i++)
		for(int j = i+1; j < meshes.size(); j++)
			if( collision_detector->find_collisions(meshes[i], meshes[j], &virt_nodes, time_step) < 0 )
				return -1;

	for(int i = 0; i < meshes.size(); i++)
		if (meshes[i]->do_next_step(time_step) < 0)
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

ElasticNode* TetrMeshSet::getNode(int num)
{
	if(num < 0 || num >= virt_nodes.size())
		return NULL;
	return &virt_nodes[num];
};

float TetrMeshSet::get_max_possible_tau()
{
	float tau = meshes[0]->get_max_possible_tau();
	for(int i = 1; i < meshes.size(); i++) {
		float ftmp = meshes[i]->get_max_possible_tau();
		if(ftmp < tau)
			tau = ftmp;
	}

	return tau;
}

TetrMesh_1stOrder* TetrMeshSet::get_mesh_by_zone_num(int zone_num)
{
	for (int i = 0; i < meshes.size(); i++)
		if (meshes[i]->zone_num == zone_num)
			return meshes[i];
}
