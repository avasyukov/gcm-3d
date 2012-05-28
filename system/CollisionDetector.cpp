#include "CollisionDetector.h"

CollisionDetector::CollisionDetector()
{
	logger = NULL;
	data_bus = NULL;
	mesh_set = NULL;
}

CollisionDetector::~CollisionDetector() {
	//
}

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
}

void CollisionDetector::attach(DataBus* new_data_bus)
{
	data_bus = new_data_bus;
}

void CollisionDetector::attach(TetrMeshSet* new_mesh_set)
{
	mesh_set = new_mesh_set;
}

void CollisionDetector::set_treshold(float value)
{
	treshold = value;
}

float CollisionDetector::get_treshold()
{
	return treshold;
}

bool CollisionDetector::find_intersection(MeshOutline &outline1, MeshOutline &outline2, MeshOutline &intersection)
{

	// check for intersection
	for(int j = 0; j < 3; j++) {
		intersection.min_coords[j] = fmaxf(outline1.min_coords[j] - treshold, outline2.min_coords[j] - treshold);
		intersection.max_coords[j] = fminf(outline1.max_coords[j] + treshold, outline2.max_coords[j] + treshold);
		if(intersection.min_coords[j] > intersection.max_coords[j])
			return false;
	}
	
	return true;

}

void CollisionDetector::find_collisions(vector<ElasticNode> &virt_nodes)
{
	if( data_bus == NULL )
		throw GCMException( GCMException::COLLISION_EXCEPTION, "DataBus is not attached!");
	if( mesh_set == NULL )
		throw GCMException( GCMException::COLLISION_EXCEPTION, "MeshSet is not attached!");

	MeshOutline intersection;

	vector<Triangle> local_faces;

	data_bus->sync_outlines();

	vector<ElasticNode> local_nodes;
	int procs_to_sync = data_bus->get_procs_total_num();

	*logger < "Processing local/local collisions";

	// process collisions between local nodes and local faces
	// we start both cycles from zero because collision should be 'symmetric'
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
		for (int j = 0; j < mesh_set->get_number_of_local_meshes(); j++)
		{
			TetrMesh_1stOrder* mesh1 = mesh_set->get_local_mesh(i);
			TetrMesh_1stOrder* mesh2 = mesh_set->get_local_mesh(j);

			if ( ( i != j ) && ( find_intersection(mesh1->outline, mesh2->outline, intersection) ) )
			{
				*logger << "Collision detected between local mesh zone #" << mesh1->zone_num 
						<< " and local mesh zone #" < mesh2->zone_num;
				// find local nodes inside intersection
				find_nodes_in_intersection(mesh1->nodes, intersection, local_nodes);
				// find local faces inside intersection
				find_faces_in_intersection(mesh2->border, mesh2->nodes, intersection, local_faces);
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

						if( mesh1->vector_intersects_triangle( 
								mesh2->nodes[ local_faces[l].vert[0] ].coords,
								mesh2->nodes[ local_faces[l].vert[1] ].coords,
								mesh2->nodes[ local_faces[l].vert[2] ].coords,
								local_nodes[k].coords,
								direction, get_treshold(), new_node.coords ) )
						{
							( mesh1->nodes[ local_nodes[k].local_num ] ).contact_type = IN_CONTACT;
							( mesh1->nodes[ local_nodes[k].local_num ] ).contact_data->axis_plus[0] 
											= virt_nodes.size();

							mesh2->interpolate_triangle(
								mesh2->nodes[ local_faces[l].vert[0] ].coords,
								mesh2->nodes[ local_faces[l].vert[1] ].coords,
								mesh2->nodes[ local_faces[l].vert[2] ].coords,
								new_node.coords,
								mesh2->nodes[ local_faces[l].vert[0] ].values,
								mesh2->nodes[ local_faces[l].vert[1] ].values,
								mesh2->nodes[ local_faces[l].vert[2] ].values,
								new_node.values);

							// remote_num here should be remote face (!) num
							new_node.remote_zone_num = mesh2->zone_num;
							new_node.remote_num = local_faces[l].local_num;
							// remember real remote num of one of verticles
							new_node.absolute_num = mesh2->nodes[ local_faces[l].vert[0] ].local_num;

							virt_nodes.push_back(new_node);

							break;
						}
					}
				}

				// clear
				local_nodes.clear();
				local_faces.clear();
			}
		}

	*logger < "Local/local collisions processed";
	
	MeshOutline **inters = new MeshOutline*[mesh_set->get_number_of_local_meshes()];
	int **fs = new int*[mesh_set->get_number_of_local_meshes()];
	int **fl = new int*[mesh_set->get_number_of_local_meshes()];
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		inters[i] = new MeshOutline[mesh_set->get_number_of_remote_meshes()];
		fs[i] = new int[mesh_set->get_number_of_remote_meshes()];
		fl[i] = new int[mesh_set->get_number_of_remote_meshes()];
	}

	// process collisions between local nodes and remote faces
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
		for (int j = 0; j < mesh_set->get_number_of_remote_meshes(); j++)
			if (find_intersection(mesh_set->get_local_mesh(i)->outline, mesh_set->get_remote_mesh(j)->outline, inters[i][j]))
				*logger << "Collision detected between local mesh zone #" << mesh_set->get_local_mesh(i)->zone_num 
							<< " and remote mesh zone #" < mesh_set->get_remote_mesh(j)->zone_num;
			else
				inters[i][j].min_coords[0] = inters[i][j].max_coords[0] = 0.0;
		
	data_bus->sync_faces_in_intersection(inters, fs, fl);

	for (int i = 0; i < mesh_set->get_number_of_remote_meshes(); i++)
		renumber_surface(mesh_set->get_remote_mesh(i)->border, mesh_set->get_remote_mesh(i)->nodes);
		
	ElasticNode *remote_nodes;
	Triangle *remote_faces;
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
		for (int j = 0; j < mesh_set->get_number_of_remote_meshes(); j++)	
			if (inters[i][j].min_coords[0] != inters[i][j].max_coords[0])
			{
				TetrMesh_1stOrder* loc_mesh = mesh_set->get_local_mesh(i);
				TetrMesh_1stOrder* rem_mesh = mesh_set->get_remote_mesh(j);

				// find local nodes inside intersection
				find_nodes_in_intersection(loc_mesh->nodes, inters[i][j], local_nodes);

				*logger << "Got " << local_nodes.size() << " local nodes, " << fl[i][j] < " remote faces";

				// process collisions
				ElasticNode new_node;
				float direction[3];
				basis *local_basis;
				remote_faces = &rem_mesh->border[0];
				remote_nodes = &rem_mesh->nodes[0];

				// debug print for faces sync
				/* for( int k = 0; k < local_nodes.size(); k++ )
				* 	*logger << "Node " << k << " coords: " << local_nodes[k].coords[0] << " "
				* 			<< local_nodes[k].coords[1] << " " < local_nodes[k].coords[2];
				* for( int k = 0; k < fl[i][j]; k++ )
				* 	*logger << "Face " << k << " verts: " << remote_faces[k].vert[0] << " " 
				* 			<< remote_faces[k].vert[1] << " " < remote_faces[k].vert[2];
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

						if( loc_mesh->vector_intersects_triangle( 
								remote_nodes[ remote_faces[l].vert[0] ].coords,
								remote_nodes[ remote_faces[l].vert[1] ].coords,
								remote_nodes[ remote_faces[l].vert[2] ].coords,
								local_nodes[k].coords,
								direction, get_treshold(), new_node.coords ) )
						{
							( loc_mesh->nodes[ local_nodes[k].local_num ] ).contact_type = IN_CONTACT;
							( loc_mesh->nodes[ local_nodes[k].local_num ] ).contact_data->axis_plus[0] 
											= virt_nodes.size();

							loc_mesh->interpolate_triangle(
								remote_nodes[ remote_faces[l].vert[0] ].coords,
								remote_nodes[ remote_faces[l].vert[1] ].coords,
								remote_nodes[ remote_faces[l].vert[2] ].coords,
								new_node.coords,
								remote_nodes[ remote_faces[l].vert[0] ].values,
								remote_nodes[ remote_faces[l].vert[1] ].values,
								remote_nodes[ remote_faces[l].vert[2] ].values,
								new_node.values);

							// remote_num here should be remote face (!) num
							new_node.remote_zone_num = rem_mesh->zone_num;
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

	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		delete[] inters[i];
		delete[] fs[i];
		delete[] fl[i];
	}
	delete[] inters;
	delete[] fs;
	delete[] fl;
	
	*logger < "Local/remote collisions processed";

}

void CollisionDetector::find_nodes_in_intersection(vector<ElasticNode> &nodes, MeshOutline &intersection, vector<ElasticNode> &result)
{
	for (int i = 0; i < nodes.size(); i++)
		// FIXME
		// only local nodes?
		if (nodes[i].placement_type == LOCAL)
		{
			bool flag = true;
			for (int j = 0; j < 3; j++)
				if (nodes[i].coords[j] < intersection.min_coords[j] || nodes[i].coords[j] > intersection.max_coords[j])
				{
					flag = false;
					break;
				}
			if (flag)
				result.push_back(nodes[i]);
		}
}

void CollisionDetector::find_faces_in_intersection(vector<Triangle> &faces, vector<ElasticNode> &nodes, MeshOutline &intersection, vector<Triangle> &result)
{
	for (int i = 0; i < faces.size(); i++)
	{
		int verts = 0;
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				if (nodes[faces[i].vert[j]].coords[k] < intersection.min_coords[k] || nodes[faces[i].vert[j]].coords[k] > intersection.max_coords[k])
				{
					verts++;
					break;
				}
		if (verts != 3)
			result.push_back(faces[i]);
	}
}

void CollisionDetector::find_faces_in_intersection(vector<Triangle> &faces, vector<ElasticNode> &nodes, MeshOutline &intersection, vector<int> &result)
{
	for (int i = 0; i < faces.size(); i++)
	{
		int verts = 0;
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				if (nodes[faces[i].vert[j]].coords[k] < intersection.min_coords[k] || nodes[faces[i].vert[j]].coords[k] > intersection.max_coords[k])
				{
					verts++;
					break;
				}
		if (verts != 3)
			result.push_back(i);
	}
}

void CollisionDetector::renumber_surface(vector<Triangle> &faces, vector<ElasticNode> &nodes)
{
	int max_node_num = -1;
	for(int k = 0; k < nodes.size(); k++)
		if( nodes[k].local_num > max_node_num )
			max_node_num = nodes[k].local_num;

	int *renum = (int*) malloc( max_node_num * sizeof(int) );
	memset(renum, 0, max_node_num * sizeof(int));

	for(int k = 0; k < nodes.size(); k++)
		renum[ nodes[k].local_num ] = k + 1;	// +1 to avoid misinterpreting with zeroed memory

	for(int i = 0; i < faces.size(); i++) {
		for(int j = 0; j < 3; j++) {
			faces[i].vert[j] = renum[ faces[i].vert[j] ] - 1;
			if( faces[i].vert[j] < 0 )
				throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not create correct numbering for surface");
		}
	}

	free(renum);
}

void CollisionDetector::renumber_volume(vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes)
{
	int max_node_num = -1;
	for(int k = 0; k < nodes.size(); k++)
		if( nodes[k].local_num > max_node_num )
			max_node_num = nodes[k].local_num;

	int *renum = (int*) malloc( max_node_num * sizeof(int) );
	memset(renum, 0, max_node_num * sizeof(int));

	for(int k = 0; k < nodes.size(); k++)
		renum[ nodes[k].local_num ] = k + 1;	// +1 to avoid misinterpreting with zeroed memory

	for(int i = 0; i < tetrs.size(); i++)
	{
		tetrs[i].absolute_num = tetrs[i].local_num;
		tetrs[i].local_num = i;
		for(int j = 0; j < 4; j++) {
			tetrs[i].vert[j] = renum[ tetrs[i].vert[j] ] - 1;
			if( tetrs[i].vert[j] < 0 )
				throw GCMException( GCMException::COLLISION_EXCEPTION, "Can't create correct numbering for volume");
		}
	}

	for(int i = 0; i < nodes.size(); i++)
	{
		nodes[i].absolute_num = nodes[i].local_num;
		nodes[i].local_num = i;
		nodes[i].placement_type = LOCAL;
	}

	free(renum);
}
