#include "CollisionDetectorForLayers.h"

CollisionDetectorForLayers::CollisionDetectorForLayers()
{
	index_shift = -1;
	shift_direction = -1;
};

CollisionDetectorForLayers::~CollisionDetectorForLayers() { };

void CollisionDetectorForLayers::find_collisions(vector<ElasticNode> &virt_nodes)
{
	vector<int> local_nodes;

	*logger < "Processing collisions";

	// sync meshes outlines
	data_bus->sync_outlines();

	MeshOutline **inters = new MeshOutline*[mesh_set->get_number_of_local_meshes()];
	int **fs = new int*[mesh_set->get_number_of_local_meshes()];
	int **fl = new int*[mesh_set->get_number_of_local_meshes()];
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		inters[i] = new MeshOutline[mesh_set->get_number_of_remote_meshes()];
		fs[i] = new int[mesh_set->get_number_of_remote_meshes()];
		fl[i] = new int[mesh_set->get_number_of_remote_meshes()];
	}

	// check outlines to determine possible collisions
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
		for (int j = 0; j < mesh_set->get_number_of_remote_meshes(); j++)
			if (find_intersection(mesh_set->get_local_mesh(i)->outline, mesh_set->get_remote_mesh(j)->outline, inters[i][j]))
				*logger << "Collision detected between mesh zone #" << mesh_set->get_local_mesh(i)->zone_num 
							<< " and mesh zone #" < mesh_set->get_remote_mesh(j)->zone_num;
			else
				inters[i][j].min_coords[0] = inters[i][j].max_coords[0] = 0.0;

	// sync and process faces in intersection
	data_bus->sync_faces_in_intersection(inters, fs, fl);

	for (int i = 0; i < mesh_set->get_number_of_remote_meshes(); i++)
		renumber_surface(mesh_set->get_remote_mesh(i)->border, mesh_set->get_remote_mesh(i)->nodes);

	// process collisions
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
		for (int j = 0; j < mesh_set->get_number_of_remote_meshes(); j++)	
			if( inters[i][j].min_coords[0] != inters[i][j].max_coords[0] )
			{
				TetrMesh_1stOrder* mesh1 = mesh_set->get_local_mesh(i);
				TetrMesh_1stOrder* mesh2 = mesh_set->get_remote_mesh(j);

				// If it is the first run - process mesh once and find and remember index_shift
				if(index_shift < 0 )
					find_index_shift(mesh1, mesh2);

				// Find nodes in intersection and process them
				find_nodes_in_intersection(mesh1->nodes, inters[i][j], local_nodes);
//for(int z = 0; z < local_nodes.size(); z++)
//	*logger << "LOCAL NODE: " < local_nodes[z];
				process_mesh(local_nodes, mesh1, mesh2, virt_nodes);

				local_nodes.clear();
			}

	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		delete[] inters[i];
		delete[] fs[i];
		delete[] fl[i];
	}
	delete[] inters;
	delete[] fs;
	delete[] fl;

	*logger < "Collisions processed";
};

void CollisionDetectorForLayers::find_index_shift(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2)
{
	float h = mesh1->get_min_h() / 2;
	MeshOutline outline1 = mesh1->outline;
	MeshOutline outline2 = mesh2->outline;

	for(int i = 0; i < 3; i++)
		if( fabs(outline1.min_coords[i] - outline2.min_coords[i]) > h )
			shift_direction = i;

	if(shift_direction < 0)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not determine shift direction");

	float shift_value = fabs( outline1.min_coords[shift_direction] - outline2.min_coords[shift_direction] );
	if( fabs( outline1.max_coords[shift_direction] - outline1.min_coords[shift_direction] - shift_value ) > h )
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Shift value does not match layer width");

	int base_index = -1;

	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		if( fabs( (mesh1->nodes).at(i).coords[shift_direction] - outline1.min_coords[shift_direction] ) < h ) {
			base_index = i;
			break;
		}
	}

	if(base_index < 0)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not determine base index");

	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		bool is_base_pair = true;
		for(int j = 0; j < 3; j++) {
			if( ( j != shift_direction ) 
					&& ( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(base_index).coords[j] ) > h ) )
				is_base_pair = false;
			if( ( j == shift_direction )
					&& ( fabs( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(base_index).coords[j] ) - shift_value ) > h ) )
				is_base_pair = false;
		}
		if( is_base_pair ) {
			index_shift = i - base_index;
			break;
		}
	}

	if(index_shift < 0)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not determine index shift");

	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		if( fabs( (mesh1->nodes).at(i).coords[shift_direction] - outline1.min_coords[shift_direction] ) < h )
		{
			for(int j = 0; j < 3; j++) {
				if( j != shift_direction ) {
					if( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(i+index_shift).coords[j] ) > h )
						throw GCMException( GCMException::COLLISION_EXCEPTION, "Bad index shift");
				} else {
					if( fabs( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(i+index_shift).coords[j] ) - shift_value ) > h )
						throw GCMException( GCMException::COLLISION_EXCEPTION, "Bad index shift");
				}
			}
		}
	}

	*logger << "DEBUG: Layer shift direction " < shift_direction;
	*logger << "DEBUG: Layer shift value " < shift_value;
	*logger << "DEBUG: Resulting node index delta " < index_shift;
};

void CollisionDetectorForLayers::process_mesh(vector<int> &nodes_vector, TetrMesh_1stOrder* current_mesh, TetrMesh_1stOrder* other_mesh, vector<ElasticNode> &virt_nodes)
{
	ElasticNode* node;
	float h = current_mesh->get_min_h() / 2;
	MeshOutline outline = current_mesh->outline;

	for(int i = 0; i < nodes_vector.size(); i++)
	{
		node = &(current_mesh->nodes).at( nodes_vector[i] );
		if( node->border_type == BORDER )
		{
			current_mesh->clear_contact_data( node );

			bool node_on_edge = false;
			for(int j = 0; j < 3; j++)
				if( ( j != shift_direction ) && ( ( fabs( node->coords[j] - outline.min_coords[j] ) < h ) 
							|| ( fabs( node->coords[j] - outline.max_coords[j] ) < h ) ) )
					node_on_edge = true;

			if( !node_on_edge )
			{
				if( fabs( node->coords[shift_direction] - outline.min_coords[shift_direction] ) < h )
				{
					node->contact_type = IN_CONTACT;
					// We always set 'axis_plus[0]' (!!!) because the first axis goes along _outer_ normal
					node->contact_data->axis_plus[0] = virt_nodes.size();
					add_node_by_local_num(other_mesh, nodes_vector[i] + index_shift, virt_nodes);
				}
				else if( fabs( node->coords[shift_direction] - outline.max_coords[shift_direction] ) < h )
				{
					node->contact_type = IN_CONTACT;
					node->contact_data->axis_plus[0] = virt_nodes.size();
					add_node_by_local_num(other_mesh, nodes_vector[i] - index_shift, virt_nodes);
				}
			}
		}
	}

};

void CollisionDetectorForLayers::add_node_by_local_num(TetrMesh_1stOrder* other_mesh, int local_num, vector<ElasticNode> &virt_nodes)
{
	ElasticNode new_node;
	int index = -1;
//*logger << "DEBUG: looking for node with local_num " << local_num << " other_mesh nodes: " < (other_mesh->nodes).size();
	for(int i = 0; i < (other_mesh->nodes).size(); i++) {
//*logger << "DEBUG: remote node local num " < (other_mesh->nodes).at(i).local_num;
		if((other_mesh->nodes).at(i).local_num == local_num) {
			index = i;
			break;
		}
	}
	if(index == -1)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not find node in remote mesh!");

	new_node = (other_mesh->nodes).at(index);

	// remote_num here should be remote face (!) num
	new_node.remote_zone_num = other_mesh->zone_num;
	for(int i = 0; i < (other_mesh->border).size(); i++)
	{
		bool flag = false;
		for(int j = 0; j < 3; j++)
			if( (other_mesh->border)[i].vert[j] == index )
				flag = true;
		if(flag) {
			new_node.remote_num = (other_mesh->border)[i].local_num;
			break;
		}
	}

	// remember real remote num of one of verticles - this node itself should work
	new_node.absolute_num = new_node.local_num;

	virt_nodes.push_back(new_node);
};
