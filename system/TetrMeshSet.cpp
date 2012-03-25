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
	for(int i = 0; i < meshes.size(); i++)\
		*logger << "Mesh #" << i << ". Type: "< *(meshes[i]->get_mesh_type());
};

void TetrMeshSet::log_meshes_stats()
{
	for(int i = 0; i < meshes.size(); i++) {
		*logger << "Mesh #" << i < ". Stats: ";
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
		if( fabs(time - meshes[i]->get_current_time()) > 0.001 * time ) // TODO - remove magic number
			throw GCMException( GCMException::SYNC_EXCEPTION, "Meshes report different times");
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

//	// Clear virtual nodes because they change between time steps
//	virt_nodes.clear();
//	for(int i = 0; i < meshes.size(); i++)
//		for(int j = i+1; j < meshes.size(); j++)
//			if( collision_detector->find_collisions(meshes[i], meshes[j], &virt_nodes, time_step) < 0 )
//				return -1;

	// Clear virtual nodes because they change between time steps
	virt_nodes.clear();

	// get outlines for all meshes
	// FIXME
	// should we use vector<MeshOutline*> here?
	//
	// FIXME
	// all collisions-related code should be moved to the method of
	// CollisionDetector implementation; a proper class hierarchy should be
	// introduced
	vector<MeshOutline> local, remote;
	MeshOutline intersection;
	vector<MeshOutlineInfo> info;

	vector<ElasticNode> remote_nodes;
	vector<Triangle> local_faces, remote_faces;

	for (int i = 0; i < meshes.size(); i++)
		local.push_back(meshes[i]->outline);

	data_bus->sync_outlines(local, remote, info);

	vector<ElasticNode> local_nodes;
	int procs_to_sync = data_bus->get_procs_total_num();

	*logger < "Processing local/local collisions";

	// process collisions between local nodes and local faces
	// we start both cycles from zero because collision should be 'symmetric'
	for (int i = 0; i < local.size(); i++)
		for (int j = 0; j < local.size(); j++)
			if ( ( i != j ) && ( collision_detector->find_intersection(local[i], local[j], intersection) ) )
			{
				*logger << "Collision detected between local mesh zone #" << meshes[i]->zone_num << " and local mesh zone #" < meshes[j]->zone_num;
				// find local nodes inside intersection
				collision_detector->find_nodes_in_intersection(meshes[i]->nodes, intersection, local_nodes);
				// find local faces inside intersection
				collision_detector->find_faces_in_intersection(meshes[j]->border, meshes[j]->nodes, intersection, local_faces);
				*logger << "Got " << local_nodes.size() << " nodes and " << local_faces.size() < " local faces";

				// process collisions
				ElasticNode new_node;
				float direction[3];
				basis *local_basis;
				for(int k = 0; k < local_nodes.size(); k++) {
					for(int l = 0; l < local_faces.size(); l++) {

						local_basis = local_nodes[k].local_basis;
						direction[0] = local_basis->ksi[0][0];
						direction[1] = local_basis->ksi[0][1];
						direction[2] = local_basis->ksi[0][2];

						if( meshes[i]->vector_intersects_triangle( 
								meshes[j]->nodes[ local_faces[l].vert[0] ].coords,
								meshes[j]->nodes[ local_faces[l].vert[1] ].coords,
								meshes[j]->nodes[ local_faces[l].vert[2] ].coords,
								local_nodes[k].coords,
								direction, collision_detector->get_treshold(), new_node.coords ) )
						{
							( meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_type = IN_CONTACT;
							( meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_data->axis_plus[0] 
											= virt_nodes.size();

							meshes[i]->interpolate_triangle(
								meshes[j]->nodes[ local_faces[l].vert[0] ].coords,
								meshes[j]->nodes[ local_faces[l].vert[1] ].coords,
								meshes[j]->nodes[ local_faces[l].vert[2] ].coords,
								new_node.coords,
								meshes[j]->nodes[ local_faces[l].vert[0] ].values,
								meshes[j]->nodes[ local_faces[l].vert[1] ].values,
								meshes[j]->nodes[ local_faces[l].vert[2] ].values,
								new_node.values);

							// remote_num here should be remote face (!) num
							new_node.remote_zone_num = meshes[j]->zone_num;
							new_node.remote_num = local_faces[l].local_num;
							// remember real remote num of one of verticles
							new_node.absolute_num = meshes[j]->nodes[ local_faces[l].vert[0] ].local_num;

							virt_nodes.push_back(new_node);

							break;
						}
					}
				}

				// clear
				local_nodes.clear();
				local_faces.clear();
			}

	*logger < "Local/local collisions processed";

	// process collisions between local nodes and remote faces
	for (int i = 0; i < local.size(); i++)
		for (int j = 0; j < remote.size(); j++)
			if (collision_detector->find_intersection(local[i], remote[j], intersection))
			{
				*logger << "Collision detected between local mesh zone #" << meshes[i]->zone_num << " and remote mesh zone #" < info[j].zone_num;
				// find local nodes inside intersection
				collision_detector->find_nodes_in_intersection(meshes[i]->nodes, intersection, local_nodes);
				// get remote faces inside intersection
				data_bus->get_remote_faces_in_intersection(info[j].proc_num, info[j].zone_num, intersection, remote_nodes, remote_faces, procs_to_sync);
				*logger << "Got " << local_nodes.size() << " local nodes, " << remote_faces.size() << " remote faces and " << remote_nodes.size() < " remote nodes";

				collision_detector->renumber_surface(remote_faces, remote_nodes);

				// process collisions
				ElasticNode new_node;
				float direction[3];
				basis *local_basis;
				for(int k = 0; k < local_nodes.size(); k++) {
					for(int l = 0; l < remote_faces.size(); l++) {

						local_basis = local_nodes[k].local_basis;
						direction[0] = local_basis->ksi[0][0];
						direction[1] = local_basis->ksi[0][1];
						direction[2] = local_basis->ksi[0][2];

						if( meshes[i]->vector_intersects_triangle( 
								remote_nodes[ remote_faces[l].vert[0] ].coords,
								remote_nodes[ remote_faces[l].vert[1] ].coords,
								remote_nodes[ remote_faces[l].vert[2] ].coords,
								local_nodes[k].coords,
								direction, collision_detector->get_treshold(), new_node.coords ) )
						{
							( meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_type = IN_CONTACT;
							( meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_data->axis_plus[0] 
											= virt_nodes.size();

							meshes[i]->interpolate_triangle(
								remote_nodes[ remote_faces[l].vert[0] ].coords,
								remote_nodes[ remote_faces[l].vert[1] ].coords,
								remote_nodes[ remote_faces[l].vert[2] ].coords,
								new_node.coords,
								remote_nodes[ remote_faces[l].vert[0] ].values,
								remote_nodes[ remote_faces[l].vert[1] ].values,
								remote_nodes[ remote_faces[l].vert[2] ].values,
								new_node.values);

							// remote_num here should be remote face (!) num
							new_node.remote_zone_num = info[j].zone_num;
							new_node.remote_num = remote_faces[l].local_num;
							// remember real remote num of one of verticles
							new_node.absolute_num = remote_nodes[ remote_faces[l].vert[0] ].local_num;

							virt_nodes.push_back(new_node);

							break;
						}
					}
				}

				// clear
				local_nodes.clear();
				remote_nodes.clear();
				remote_faces.clear();
			}

	*logger < "Local/remote collisions processed";

	// remote faces sync done, notify
	procs_to_sync--;
	data_bus->remote_faces_sync_done();
	
	// FIXME
	// do we really need this or destructor does it?
	// clear
	local.clear();
	remote.clear();
	info.clear();

	// wait for other procs
	// FIXME
	// move all MPI related code to...DataBus?
	while (procs_to_sync > 0)
	{
		MPI::Status status;
		data_bus->check_messages_sync(MPI::ANY_SOURCE, DataBus::TAG_CLASS_SYNC_FACES, status);
		// FIXME
		// we do not actually need an access to nodes/faces here
		data_bus->process_faces_sync_message(status.Get_source(), status.Get_tag(), remote_nodes, remote_faces, procs_to_sync);
	}

	*logger < "Remote faces sync done";

	// FIXME - we should do part_step(s) for all meshes in course (!!!)
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
};

TetrMesh_1stOrder* TetrMeshSet::get_mesh_by_zone_num(int zone_num)
{
	for (int i = 0; i < meshes.size(); i++)
		if (meshes[i]->zone_num == zone_num)
			return meshes[i];
	return NULL;
};

void TetrMeshSet::pre_process_meshes()
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i]->pre_process_mesh();
};
