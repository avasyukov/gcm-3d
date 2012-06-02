#include "TetrMeshSet.h"

TetrMeshSet::TetrMeshSet()
{
	data_bus = NULL;
	stresser = NULL;
	rheology = NULL;
	numerical_method = NULL;
	collision_detector = NULL;
};

TetrMeshSet::~TetrMeshSet() { };

void TetrMeshSet::attach(TetrMesh_1stOrder* new_mesh)
{
	if(new_mesh != NULL) {
		new_mesh->attach(stresser);
		new_mesh->attach(rheology);
	}
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
	// We need this 0.99 to avoid false 'outer characteristics' exception because of floating point operations rounding errors.
	// This happen when the point should be exactly on the face of the tetrahedron.
	float time_step = 0.99 * data_bus->get_max_possible_tau(get_max_possible_tau());

	// Static detector means you are sure virt nodes don't change between time steps and there is no need to recalculate them
	// Static collision detector is run only once at the first time step
	// Otherwise - clear the list and recalc
	if( (! collision_detector->is_static() ) || (get_current_time() == 0.0) )
	{
		// Clear virtual nodes because they change between time steps
		virt_nodes.clear();
		// Find collisions and fill virtual nodes
		collision_detector->find_collisions(virt_nodes);
	}

	*logger << "Virtual nodes: " < virt_nodes.size();

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

		// Sync parts of remote meshes that are required for virtual nodes
		sync_remote_data();
		// Sync remote nodes in local meshes
		data_bus->sync_nodes();

		// calculation for local meshes
		for(int l = 0; l < local_meshes.size(); l++)
		{
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

void TetrMeshSet::sync_remote_data()
{
	for (int r = 0; r < remote_meshes.size(); r++)
		remote_meshes[r]->clear_data();

	data_bus->sync_tetrs();

	// process synced data for remote meshes
	for (int r = 0; r < remote_meshes.size(); r++)
	{
		*logger << "Remote mesh #" << remote_meshes[r]->zone_num << " Pre-processing virtual mesh. "
				<< "Tetrs: " << (remote_meshes[r]->tetrs).size() << " Nodes: " < (remote_meshes[r]->nodes).size();

		collision_detector->renumber_volume(remote_meshes[r]->tetrs, remote_meshes[r]->nodes);
		remote_meshes[r]->pre_process_mesh();

		// debug print for tetr sync
//		*logger << "BORDER size " < remote_meshes[r]->border.size();
//		for(int i = 0; i < (remote_meshes[r]->nodes).size(); i++)
//			*logger << "Node " << i << " abs num " << (remote_meshes[r]->nodes)[i].absolute_num << " coords: " 
//					<< (remote_meshes[r]->nodes)[i].coords[0] << " "
//					<< (remote_meshes[r]->nodes)[i].coords[1] << " " < (remote_meshes[r]->nodes)[i].coords[2];
	}

	*logger << "Preparing lookups for virt nodes linking. Number of meshes: " < get_number_of_meshes();
	// create renum mapping for fast renum
	int **renum = new int*[get_number_of_meshes()];
	for (int i = 0; i < get_number_of_meshes(); i++)
	{
		int zone_num = meshes[i].zone_num;
		*logger << "Preparing lookups for mesh #" << i << " with zone_num " < zone_num;

		int max_node_num = -1;
		for(int k = 0; k < meshes[i].nodes.size(); k++)
			if( meshes[i].nodes[k].absolute_num > max_node_num )
				max_node_num = meshes[i].nodes[k].absolute_num;

		*logger << "Mesh #" << i << " Max absolute num: " < max_node_num;

		if(max_node_num >= 0) {
			renum[zone_num] = new int[max_node_num + 1];
			memset(renum[zone_num], 0, (max_node_num + 1) * sizeof(int));
		} else {
			renum[zone_num] = NULL;
		}

		for(int k = 0; k < meshes[i].nodes.size(); k++)
			renum[zone_num][ meshes[i].nodes[k].absolute_num ] = k + 1;	// +1 to avoid misinterpreting with zeroed memory
	}

	// link data for local meshes
	for(int l = 0; l < local_meshes.size(); l++)
	{
		// link virtual nodes for this mesh
		*logger << "Local mesh #" << local_meshes[l]->zone_num < " Linking virt nodes to remote mesh";

		for(int i = 0; i < local_meshes[l]->nodes.size(); i++)
		{
			if( (local_meshes[l]->nodes[i].border_type == BORDER) 
				&& (local_meshes[l]->nodes[i].contact_data->axis_plus[0] != -1) )
			{
				ElasticNode *vnode = getNode( local_meshes[l]->nodes[i].contact_data->axis_plus[0] );
				vnode->mesh = get_mesh_by_zone_num(vnode->remote_zone_num);
				if(vnode->mesh == NULL)
					throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't find remote zone for vnode");

				int origin_index = renum[ vnode->remote_zone_num ][ vnode->absolute_num ] - 1;

				if( origin_index < 0 )
					throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't find virt node origin");

				vnode->placement_type = LOCAL;
				vnode->border_type = BORDER;
				vnode->contact_type = IN_CONTACT;
				vnode->elements = (vnode->mesh->nodes)[origin_index].elements;
				vnode->border_elements = (vnode->mesh->nodes)[origin_index].border_elements;
				vnode->local_num = (vnode->mesh->nodes)[origin_index].local_num;		
			}
		}

		*logger << "Local mesh #" << local_meshes[l]->zone_num < " Linking virt nodes done";
	}

	for (int i = 0; i < get_number_of_meshes(); i++)
		if(renum[i] != NULL)
			delete[] renum[i];

	delete[] renum;
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
		meshes_at_proc[zones_info[i]]++;
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

TetrMeshSet* TetrMeshSet::getInstance()
{
	static TetrMeshSet ms;
	return &ms;
}

void TetrMeshSet::init()
{
	data_bus = DataBus::getInstance();
}