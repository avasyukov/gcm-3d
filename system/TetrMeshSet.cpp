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

	for(int i = 0; i < local_meshes.size(); i++)
		local_meshes[i]->attach(logger);
};

void TetrMeshSet::attach(TetrMesh_1stOrder* new_mesh)
{
	if(new_mesh != NULL) {
		new_mesh->attach(logger);
//		new_mesh->attach_data_bus(data_bus);
		new_mesh->attach(stresser);
		new_mesh->attach(rheology);
		// FIXME - never ever do it! We need separate copy of method class for each mesh - see below.
		// new_mesh->attach(numerical_method);
		new_mesh->attach(this);
//		meshes.push_back(new_mesh);
//		if (new_mesh->local)
//			local_meshes.push_back(new_mesh);
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

//	for(int i = 0; i < local_meshes.size(); i++)
//		local_meshes[i]->attach_data_bus(new_data_bus);

//	// initialize remote nodes store
//	FIXME
//	for (i = 0; i < data_bus->get_total_proc_num(); i++)
//		remote_nodes.push_back(vector<ElasticNode*>());

};

void TetrMeshSet::attach(Stresser* new_stresser)
{
	stresser = new_stresser;

	for(int i = 0; i < local_meshes.size(); i++)
		local_meshes[i]->attach(new_stresser);
};

void TetrMeshSet::attach(RheologyCalculator* new_rheology)
{
	rheology = new_rheology;

	for(int i = 0; i < local_meshes.size(); i++)
		local_meshes[i]->attach(new_rheology);
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
	for(int i = 0; i < local_meshes.size(); i++)
		*logger << "Local mesh #" << i << ". Type: "< *(local_meshes[i]->get_mesh_type());
};

void TetrMeshSet::log_meshes_stats()
{
	for(int i = 0; i < local_meshes.size(); i++) 
	{
		*logger << "Local mesh #" << i < ". Stats: ";
			local_meshes[i]->log_mesh_stats();
	}
};

float TetrMeshSet::get_current_time()
{
	float time;
	if(local_meshes.size() == 0)
		return -1;

	time = local_meshes[0]->get_current_time();

	for(int i = 0; i < local_meshes.size(); i++) {
		if( fabs(time - local_meshes[i]->get_current_time()) > 0.001 * time ) // TODO - remove magic number
			throw GCMException( GCMException::SYNC_EXCEPTION, "Meshes report different times");
	}

	return time;
};

int TetrMeshSet::do_next_step()
{

	float time_step = data_bus->get_max_possible_tau(get_max_possible_tau());

	// Clear virtual nodes because they change between time steps
	virt_nodes.clear();

	// FIXME
	// all collisions-related code should be moved to the method of
	// CollisionDetector implementation; a proper class hierarchy should be
	// introduced
	MeshOutline intersection;

	vector<Triangle> local_faces;

	data_bus->sync_outlines();

	vector<ElasticNode> local_nodes;
	int procs_to_sync = data_bus->get_procs_total_num();

	*logger < "Processing local/local collisions";

	// process collisions between local nodes and local faces
	// we start both cycles from zero because collision should be 'symmetric'
	for (int i = 0; i < local_meshes.size(); i++)
		for (int j = 0; j < local_meshes.size(); j++)
			if ( ( i != j ) && ( collision_detector->find_intersection(local_meshes[i]->outline, local_meshes[j]->outline, intersection) ) )
			{
				*logger << "Collision detected between local mesh zone #" << local_meshes[i]->zone_num << " and local mesh zone #" < local_meshes[j]->zone_num;
				// find local nodes inside intersection
				collision_detector->find_nodes_in_intersection(local_meshes[i]->nodes, intersection, local_nodes);
				// find local faces inside intersection
				collision_detector->find_faces_in_intersection(local_meshes[j]->border, local_meshes[j]->nodes, intersection, local_faces);
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

						if( local_meshes[i]->vector_intersects_triangle( 
								local_meshes[j]->nodes[ local_faces[l].vert[0] ].coords,
								local_meshes[j]->nodes[ local_faces[l].vert[1] ].coords,
								local_meshes[j]->nodes[ local_faces[l].vert[2] ].coords,
								local_nodes[k].coords,
								direction, collision_detector->get_treshold(), new_node.coords ) )
						{
							( local_meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_type = IN_CONTACT;
							( local_meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_data->axis_plus[0] 
											= virt_nodes.size();

							local_meshes[i]->interpolate_triangle(
								local_meshes[j]->nodes[ local_faces[l].vert[0] ].coords,
								local_meshes[j]->nodes[ local_faces[l].vert[1] ].coords,
								local_meshes[j]->nodes[ local_faces[l].vert[2] ].coords,
								new_node.coords,
								local_meshes[j]->nodes[ local_faces[l].vert[0] ].values,
								local_meshes[j]->nodes[ local_faces[l].vert[1] ].values,
								local_meshes[j]->nodes[ local_faces[l].vert[2] ].values,
								new_node.values);

							// remote_num here should be remote face (!) num
							new_node.remote_zone_num = local_meshes[j]->zone_num;
							new_node.remote_num = local_faces[l].local_num;
							// remember real remote num of one of verticles
							new_node.absolute_num = local_meshes[j]->nodes[ local_faces[l].vert[0] ].local_num;

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
	
	MeshOutline **inters = new MeshOutline*[local_meshes.size()];
	int **fs = new int*[local_meshes.size()];
	int **fl = new int*[local_meshes.size()];
	for (int i = 0; i < local_meshes.size(); i++)
	{
		inters[i] = new MeshOutline[remote_meshes.size()];
		fs[i] = new int[remote_meshes.size()];
		fl[i] = new int[remote_meshes.size()];
	}

	// process collisions between local nodes and remote faces
	for (int i = 0; i < local_meshes.size(); i++)
		for (int j = 0; j < remote_meshes.size(); j++)
			if (collision_detector->find_intersection(local_meshes[i]->outline, remote_meshes[j]->outline, inters[i][j]))
				*logger << "Collision detected between local mesh zone #" << local_meshes[i]->zone_num << " and remote mesh zone #" < remote_meshes[j]->zone_num;
			else
				inters[i][j].min_coords[0] = inters[i][j].max_coords[0] = 0.0;
	
	for (int i = 0; i < remote_meshes.size(); i++)		
	{
		remote_meshes[i]->border.clear();
		remote_meshes[i]->nodes.clear();
	}
		
	data_bus->sync_faces_in_intersection(inters, fs, fl);
	for (int i = 0; i < remote_meshes.size(); i++)
		collision_detector->renumber_surface(remote_meshes[i]->border, remote_meshes[i]->nodes);
		
	ElasticNode *remote_nodes;
	Triangle *remote_faces;
	for (int i = 0; i < local_meshes.size(); i++)
		for (int j = 0; j < remote_meshes.size(); j++)	
			if (inters[i][j].min_coords[0] != inters[i][j].max_coords[0])
			{
				// find local nodes inside intersection
				collision_detector->find_nodes_in_intersection(local_meshes[i]->nodes, inters[i][j], local_nodes);
				// get remote faces inside intersection
//				data_bus->get_remote_faces_in_intersection(remote_meshes[j]->proc_num, remote_meshes[j]->zone_num, intersection, remote_nodes, remote_faces, procs_to_sync);
				*logger << "Got " << local_nodes.size() << " local nodes, " << fl[i][j] < " remote faces";

				// process collisions
				ElasticNode new_node;
				float direction[3];
				basis *local_basis;
				remote_faces = &remote_meshes[j]->border[0];
				remote_nodes = &remote_meshes[j]->nodes[0];

				// debug print for faces sync
				/* for( int k = 0; k < local_nodes.size(); k++ )
				* 	*logger << "Node " << k << " coords: " << local_nodes[k].coords[0] << " "
				* 			<< local_nodes[k].coords[1] << " " < local_nodes[k].coords[2];
				* for( int k = 0; k < fl[i][j]; k++ )
				* 	*logger << "Face " << k << " verts: " << remote_faces[k].vert[0] << " " << remote_faces[k].vert[1] << " "
				* 			< remote_faces[k].vert[2];
				* for( int k = 0; k < remote_meshes[j]->nodes.size(); k++ )
				* 	*logger << "Remote node " << k << " coords: " << remote_nodes[k].coords[0] << " "
				* 			<< remote_nodes[k].coords[1] << " " < remote_nodes[k].coords[2];
				*/

				for(int k = 0; k < local_nodes.size(); k++) {
					for(int l = fs[i][j]; l < fs[i][j]+fl[i][j]; l++) {

						local_basis = local_nodes[k].local_basis;
						direction[0] = local_basis->ksi[0][0];
						direction[1] = local_basis->ksi[0][1];
						direction[2] = local_basis->ksi[0][2];

						if( local_meshes[i]->vector_intersects_triangle( 
								remote_nodes[ remote_faces[l].vert[0] ].coords,
								remote_nodes[ remote_faces[l].vert[1] ].coords,
								remote_nodes[ remote_faces[l].vert[2] ].coords,
								local_nodes[k].coords,
								direction, collision_detector->get_treshold(), new_node.coords ) )
						{
							( local_meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_type = IN_CONTACT;
							( local_meshes[i]->nodes[ local_nodes[k].local_num ] ).contact_data->axis_plus[0] 
											= virt_nodes.size();

							local_meshes[i]->interpolate_triangle(
								remote_nodes[ remote_faces[l].vert[0] ].coords,
								remote_nodes[ remote_faces[l].vert[1] ].coords,
								remote_nodes[ remote_faces[l].vert[2] ].coords,
								new_node.coords,
								remote_nodes[ remote_faces[l].vert[0] ].values,
								remote_nodes[ remote_faces[l].vert[1] ].values,
								remote_nodes[ remote_faces[l].vert[2] ].values,
								new_node.values);

							// remote_num here should be remote face (!) num
							new_node.remote_zone_num = remote_meshes[j]->zone_num;
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
			}
	
	*logger << "Virtual nodes: " < virt_nodes.size();
	
	for (int i = 0; i < local_meshes.size(); i++)
	{
		delete[] inters[i];
		delete[] fs[i];
		delete[] fl[i];
	}
	delete[] inters;
	delete[] fs;
	delete[] fl;
	
	*logger < "Local/remote collisions processed";

	// number of stages for current numerical method
	int number_of_stages;

	// perform checks
	for(int i = 0; i < local_meshes.size(); i++)
	{
		if( local_meshes[i]->method == NULL )
			throw GCMException( GCMException::MESH_EXCEPTION, "No NM attached");

		if( local_meshes[i]->rheology == NULL )
			throw GCMException( GCMException::MESH_EXCEPTION, "No RC attached");

		if( time_step < 0 )
			throw GCMException( GCMException::MESH_EXCEPTION, "Time step is negative");

		if( (number_of_stages = local_meshes[i]->method->get_number_of_stages()) <= 0 )
			throw GCMException( GCMException::MESH_EXCEPTION, "Incorrect number of stages");
	}

	// set external stress
	for(int i = 0; i < local_meshes.size(); i++)
		if( local_meshes[i]->set_stress( get_current_time() ) < 0 )
			throw GCMException( GCMException::MESH_EXCEPTION, "Set stress failed");

	// do part steps
	for(int s = 0; s < number_of_stages; s++)
	{

		*logger << "Part step #" << s < " Syncing remote meshes";

		data_bus->sync_tetrs();

		// process synced data for remote meshes
		for (int r = 0; r < remote_meshes.size(); r++)
		{
			*logger << "Remote mesh #" << remote_meshes[r]->zone_num << " Pre-processing virtual mesh. "
					<< "Tetrs: " << (remote_meshes[r]->tetrs).size() << " Nodes: " < (remote_meshes[r]->nodes).size();

			// renumber everything
			for(int i = 0; i < (remote_meshes[r]->tetrs).size(); i++)
			{
				(remote_meshes[r]->tetrs)[i].absolute_num = (remote_meshes[r]->tetrs)[i].local_num;
				(remote_meshes[r]->tetrs)[i].local_num = i;
				for(int j = 0; j < 4; j++) {
					bool node_found = false;
					for(int k = 0; k < (remote_meshes[r]->nodes).size(); k++) {
						if((remote_meshes[r]->tetrs)[i].vert[j] == (remote_meshes[r]->nodes)[k].local_num) {
							(remote_meshes[r]->tetrs)[i].vert[j] = k;
							node_found = true;
							break;
						}
					}
					if( !node_found )
						throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't create correct numbering for volume");
				}
			}
			for(int i = 0; i < (remote_meshes[r]->nodes).size(); i++)
			{
				(remote_meshes[r]->nodes)[i].absolute_num = (remote_meshes[r]->nodes)[i].local_num;
				(remote_meshes[r]->nodes)[i].local_num = i;
			}

			// create links, etc
			remote_meshes[r]->attach(logger);
			for(int i = 0; i < (remote_meshes[r]->nodes).size(); i++)
				(remote_meshes[r]->nodes)[i].placement_type = LOCAL;
			remote_meshes[r]->pre_process_mesh();

			// debug print for tetr sync
//			*logger << "BORDER size " < remote_meshes[r]->border.size();
//			for(int i = 0; i < (remote_meshes[r]->nodes).size(); i++)
//				*logger << "Node " << i << " abs num " << (remote_meshes[r]->nodes)[i].absolute_num << " coords: " 
//						<< (remote_meshes[r]->nodes)[i].coords[0] << " "
//						<< (remote_meshes[r]->nodes)[i].coords[1] << " " < (remote_meshes[r]->nodes)[i].coords[2];
		}

		// calculation for local meshes
		for(int l = 0; l < local_meshes.size(); l++)
		{
			// link virtual nodes for this mesh
			*logger << "Mesh " << local_meshes[l]->zone_num << " Stage " << s < " Linking virt nodes to remote mesh";

			for(int i = 0; i < local_meshes[l]->nodes.size(); i++)
			{
				if(local_meshes[l]->nodes[i].border_type == BORDER)
				{
					if(local_meshes[l]->nodes[i].contact_data->axis_plus[0] != -1)
					{
						ElasticNode *vnode = getNode( local_meshes[l]->nodes[i].contact_data->axis_plus[0] );
//						*logger << "Looking for remote mesh with rnum: " < vnode->remote_zone_num;
						vnode->mesh = get_mesh_by_zone_num(vnode->remote_zone_num);
						if(vnode->mesh == NULL)
							throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't find remote zone for vnode");

//						*logger << "Looking for node with num: " < vnode->absolute_num;
						bool node_found = false;
						for(int z = 0; z < (vnode->mesh->nodes).size(); z++)
							if((vnode->mesh->nodes)[z].absolute_num == vnode->absolute_num)
							{
//						*logger << "DEBUG VNODE " << (vnode->mesh->nodes)[z].placement_type << " " 
//							<< (vnode->mesh->nodes)[z].elements->size() 
//							<< " " < (vnode->mesh->nodes)[z].border_elements->size();
								node_found = true;
								vnode->placement_type = LOCAL;
								vnode->border_type = BORDER;
								vnode->contact_type = IN_CONTACT;
								vnode->elements = (vnode->mesh->nodes)[z].elements;
								vnode->border_elements = (vnode->mesh->nodes)[z].border_elements;
								vnode->local_num = (vnode->mesh->nodes)[z].local_num;
								break;
							}
						if( !node_found )
							throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't find virt node origin");
					}
				}
			}

			// real calculation
			*logger << "Mesh " << local_meshes[l]->zone_num << " Stage " << s < " Start calculation";

			if( local_meshes[l]->do_next_part_step(time_step, s) < 0 )
				throw GCMException( GCMException::MESH_EXCEPTION, "Do next part step failed");

			*logger << "Mesh " << local_meshes[l]->zone_num << " Stage " << s < " Calculation done";
		}
	}

// FIXME
//	for(int i = 0; i < local_meshes.size(); i++)
//		local_meshes[i]->move_coords(time_step);

	for(int i = 0; i < local_meshes.size(); i++)
		if( local_meshes[i]->proceed_rheology() < 0 )
			throw GCMException( GCMException::MESH_EXCEPTION, "Proceed rheology failed");

	for(int i = 0; i < local_meshes.size(); i++)
		local_meshes[i]->update_current_time(time_step);

	return 0;
};

int TetrMeshSet::get_number_of_local_meshes()
{
	return local_meshes.size();
};

int TetrMeshSet::get_number_of_remote_meshes()
{
	return remote_meshes.size();
};

TetrMesh_1stOrder* TetrMeshSet::get_local_mesh(int num)
{
	if(num < 0 || num >= local_meshes.size())
		return NULL;
	return local_meshes[num];
};

TetrMesh_1stOrder* TetrMeshSet::get_remote_mesh(int num)
{
	if(num < 0 || num >= remote_meshes.size())
		return NULL;
	return remote_meshes[num];
};

int TetrMeshSet::get_number_of_meshes()
{
	return meshes_number;
};

TetrMesh_1stOrder* TetrMeshSet::get_mesh(int num)
{
	if(num < 0 || num >= meshes_number)
		return NULL;
	return meshes+num;
};

ElasticNode* TetrMeshSet::getNode(int num)
{
	if(num < 0 || num >= virt_nodes.size())
		return NULL;
	return &virt_nodes[num];
};

float TetrMeshSet::get_max_possible_tau()
{
	float tau = local_meshes[0]->get_max_possible_tau();
	for(int i = 1; i < local_meshes.size(); i++)
	{
		float ftmp = local_meshes[i]->get_max_possible_tau();
		if(ftmp < tau)
			tau = ftmp;
	}

	return tau;
};

TetrMesh_1stOrder* TetrMeshSet::get_mesh_by_zone_num(int zone_num)
{
	for (int i = 0; i < meshes_number; i++)
		if (meshes[i].zone_num == zone_num)
			return meshes+i;
	return NULL;
};

void TetrMeshSet::pre_process_meshes()
{
	for (int i = 0; i < local_meshes.size(); i++)
		local_meshes[i]->pre_process_mesh();
};

void TetrMeshSet::init_mesh_container(vector<int> &zones_info)
{
	meshes_number = zones_info.size();
	// allocate memory
	meshes = new TetrMesh_1stOrder[meshes_number];
	meshes_at_proc = (int*)calloc(data_bus->get_procs_total_num(), sizeof(int));
	for (int i = 0; i < meshes_number; i++)
	{
		meshes_at_proc[zones_info[i]]++;
		meshes[i].attach(logger);
	}
	int k = 0;
	for (int i = 0; i < data_bus->get_procs_total_num(); i++)
	{
		int q = 0;
		for (int j = 0; j < meshes_at_proc[i]; j++)
		{
			while (zones_info[q] != i)
				q++;
			meshes[k].zone_num = q;
			meshes[k].proc_num = i;
			meshes[k].local = (i == data_bus->get_proc_num());
			if (meshes[k].local)
				local_meshes.push_back(meshes+k);
			else
				remote_meshes.push_back(meshes+k);
			q++;
			k++;
		}
	}
}
