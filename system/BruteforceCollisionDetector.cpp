#include "BruteforceCollisionDetector.h"

BruteforceCollisionDetector::BruteforceCollisionDetector()
{
	logger = NULL;
};

BruteforceCollisionDetector::~BruteforceCollisionDetector() { };

void BruteforceCollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
};

void BruteforceCollisionDetector::set_treshold(float value)
{
	treshold = value;
};

int BruteforceCollisionDetector::find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step)
{
	mesh1_nodes.clear();
	mesh2_nodes.clear();
	mesh1_tetrs.clear();
	mesh2_tetrs.clear();

	MeshOutline outline1 = mesh1->outline;
	MeshOutline outline2 = mesh2->outline;

	*logger < "Mesh outline 1:";
    *logger << "MinX: " < outline1.min_coords[0];
    *logger << "MaxX: " < outline1.max_coords[0];
    *logger << "MinY: " < outline1.min_coords[1];
    *logger << "MaxY: " < outline1.max_coords[1];
    *logger << "MinZ: " < outline1.min_coords[2];
    *logger << "MaxZ: " < outline1.max_coords[2];

    *logger < "Mesh outline 2:";
    *logger << "MinX: " < outline2.min_coords[0];
    *logger << "MaxX: " < outline2.max_coords[0];
    *logger << "MinY: " < outline2.min_coords[1];
    *logger << "MaxY: " < outline2.max_coords[1];
    *logger << "MinZ: " < outline2.min_coords[2];
    *logger << "MaxZ: " < outline2.max_coords[2];


	MeshOutline intersect;

	bool colliding = true;
	for(int j = 0; j < 3; j++) {
		intersect.min_coords[j] = fmaxf(outline1.min_coords[j] - treshold, outline2.min_coords[j] - treshold);
		intersect.max_coords[j] = fminf(outline1.max_coords[j] + treshold, outline2.max_coords[j] + treshold);
		if(intersect.min_coords[j] > intersect.max_coords[j])
			colliding = false;
	}

	if(!colliding) {
		*logger < "Not colliding";
		return 0;
	}

	*logger < "Colliding!";
	*logger < "Intersection:";
	*logger << "MinX: " < intersect.min_coords[0];
	*logger << "MaxX: " < intersect.max_coords[0];
	*logger << "MinY: " < intersect.min_coords[1];
	*logger << "MaxY: " < intersect.max_coords[1];
	*logger << "MinZ: " < intersect.min_coords[2];
	*logger << "MaxZ: " < intersect.max_coords[2];

	// Find nodes and tetrs in intersection to check them in details later
	find_elements_in_intersect(mesh1, &intersect, &mesh1_nodes, &mesh1_tetrs);
	find_elements_in_intersect(mesh2, &intersect, &mesh2_nodes, &mesh2_tetrs);

	// Find contacts mesh1 has with mesh2
	if( process_mesh(&mesh1_nodes, mesh1, &mesh2_tetrs, mesh2, time_step, virt_nodes) < 0 )
		return -1;

	// Find contacts mesh2 has with mesh1
	if( process_mesh(&mesh2_nodes, mesh2, &mesh1_tetrs, mesh1, time_step, virt_nodes) < 0 )
		return -1;

	return 0;
};

bool BruteforceCollisionDetector::node_in_intersection(ElasticNode* node, MeshOutline* intersect)
{
	for(int j = 0; j < 3; j++)
		if( (node->coords[j] < intersect->min_coords[j]) || (node->coords[j] > intersect->max_coords[j]) )
			return false;
	return true;
};

int BruteforceCollisionDetector::process_direction(ElasticNode* _node, float move, int axis_num, vector<int>* tetrs_vector, TetrMesh_1stOrder* tetrs_mesh, vector<ElasticNode>* virt_nodes)
{
	ElasticNode node = *_node;
	node.coords[0] += move * (node.local_basis)->ksi[axis_num][0];
	node.coords[1] += move * (node.local_basis)->ksi[axis_num][1];
	node.coords[2] += move * (node.local_basis)->ksi[axis_num][2];
	for(int k = 0; k < tetrs_vector->size(); k++) {
		if( tetrs_mesh->point_in_tetr(node.coords[0], node.coords[1], node.coords[2], &((tetrs_mesh->tetrs).at( tetrs_vector->at(k) )) ) ) {
			_node->contact_type = IN_CONTACT;
			// FIXME - we should find node on the border, this approach will fail if sound speed is different in colliding bodies

			// We must use remote node here because we will need further topology information of REMOTE mesh stored in it
			ElasticNode remote_node = (tetrs_mesh->nodes).at( (tetrs_mesh->tetrs).at( tetrs_vector->at(k) ).vert[0] );
			remote_node.coords[0] = node.coords[0];
			remote_node.coords[1] = node.coords[1];
			remote_node.coords[2] = node.coords[2];
			tetrs_mesh->interpolate(&remote_node, &((tetrs_mesh->tetrs).at( tetrs_vector->at(k) )));
			virt_nodes->push_back(remote_node);
			if(move > 0) {
				_node->contact_data->axis_plus[axis_num] = virt_nodes->size()-1;
			} else {
				_node->contact_data->axis_minus[axis_num] = virt_nodes->size()-1;
			}
			break;
		}
	}
	return 0;
};

int BruteforceCollisionDetector::process_mesh(vector<int>* nodes_vector, TetrMesh_1stOrder* current_mesh, vector<int>* tetrs_vector, TetrMesh_1stOrder* other_mesh, float time_step, vector<ElasticNode>* virt_nodes)
{
	ElasticNode node;

	for(int i = 0; i < nodes_vector->size(); i++) {

		// Clear contact data
		if( (current_mesh->nodes).at( nodes_vector->at(i) ).border_type == BORDER )
			current_mesh->clear_contact_data( &(current_mesh->nodes).at( nodes_vector->at(i) ) );

		node = (current_mesh->nodes).at( nodes_vector->at(i) );

		// TODO - it's bad, we should use default impl, but default one re-randomizes axis...
		// FIXME - we temporary use minor characteristic to ensure both are always in or out of the other body
		// FIXME - rethink this criteria ASAP! In this case contact state depends on how axis are randomized.
		// TODO - looks like we fix this issue if we use basis where the first axis coincide with outer normal
		// Two parallel planes can have contacts in some points and do not have in other ones.
		float move = time_step * sqrt( node.mu / node.rho );

		// Check all axis
		// TODO - we process only first direction because it coincides with outer normal
		// FIXME - we can not be sure that exactly the first coincides with outer normal
		// for(int j = 0; j < 3; j++) {
		for(int j = 0; j < 1; j++) {
			// Check positive direction
			if( process_direction( &(current_mesh->nodes).at( nodes_vector->at(i) ), move, j, tetrs_vector, other_mesh, virt_nodes) < 0 )
				return -1;

			// Check negative direction
			if( process_direction( &(current_mesh->nodes).at( nodes_vector->at(i) ), -move, j, tetrs_vector, other_mesh, virt_nodes) < 0 )
				return -1;
		}

		// TODO - should we check outer normal in addition? It is possible that all axis are out of the neighbour body, but normal is in it.

	}
	return 0;
};

void BruteforceCollisionDetector::find_elements_in_intersect(TetrMesh_1stOrder* mesh, MeshOutline* intersect, vector<int>* nodes_vector, vector<int>* tetrs_vector)
{
	// TODO - unreadable code - rewrite it based on get_* functions
	for(int i = 0; i < (mesh->nodes).size(); i++) {
		(mesh->nodes).at(i).contact_type = FREE;
		if(node_in_intersection( &((mesh->nodes).at(i)), intersect ) ) {
			nodes_vector->push_back(i);
			for(int j = 0; j < (((mesh->nodes).at(i)).elements)->size(); j++)
				tetrs_vector->push_back( (((mesh->nodes).at(i)).elements)->at(j) );
		}
	}
};
