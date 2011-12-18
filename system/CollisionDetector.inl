CollisionDetector::CollisionDetector()
{
	logger = NULL;
};

CollisionDetector::~CollisionDetector() { };

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
};

void CollisionDetector::set_treshold(float value)
{
	treshold = value;
};

int CollisionDetector::find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step)
{
	mesh1_nodes.clear();
	mesh2_nodes.clear();
	mesh1_tetrs.clear();
	mesh2_tetrs.clear();

	MeshOutline outline1 = mesh1->outline;
	MeshOutline outline2 = mesh2->outline;

	stringstream ss;
        ss << "Mesh outline 1:" << endl
                << "MinX: " << outline1.min_coords[0] << endl
                << "MaxX: " << outline1.max_coords[0] << endl
                << "MinY: " << outline1.min_coords[1] << endl
                << "MaxY: " << outline1.max_coords[1] << endl
                << "MinZ: " << outline1.min_coords[2] << endl
                << "MaxZ: " << outline1.max_coords[2] << endl;

       ss << "Mesh outline 2:" << endl
                << "MinX: " << outline2.min_coords[0] << endl
                << "MaxX: " << outline2.max_coords[0] << endl
                << "MinY: " << outline2.min_coords[1] << endl
                << "MaxY: " << outline2.max_coords[1] << endl
                << "MinZ: " << outline2.min_coords[2] << endl
                << "MaxZ: " << outline2.max_coords[2] << endl;

	cout << ss.str();

	MeshOutline intersect;

	bool colliding = true;
	for(int j = 0; j < 3; j++) {
		intersect.min_coords[j] = fmaxf(outline1.min_coords[j] - treshold, outline2.min_coords[j] - treshold);
		intersect.max_coords[j] = fminf(outline1.max_coords[j] + treshold, outline2.max_coords[j] + treshold);
		if(intersect.min_coords[j] > intersect.max_coords[j])
			colliding = false;
	}

	if(!colliding) {
		cout << "Not colliding\n";
		return 0;
	}

	cout << "Colliding!\n";
        cout << "Intersection:" << endl
	                << "MinX: " << intersect.min_coords[0] << endl
	                << "MaxX: " << intersect.max_coords[0] << endl
	                << "MinY: " << intersect.min_coords[1] << endl
	                << "MaxY: " << intersect.max_coords[1] << endl
	                << "MinZ: " << intersect.min_coords[2] << endl
	                << "MaxZ: " << intersect.max_coords[2] << endl;

	// Find nodes and tetrs in intersection to check them in details later
	find_elements_in_intersect(mesh1, &intersect, &mesh1_nodes, &mesh1_tetrs);
	find_elements_in_intersect(mesh2, &intersect, &mesh2_nodes, &mesh2_tetrs);

	// Find contacts mesh1 has with mesh2
	process_mesh(&mesh1_nodes, mesh1, &mesh2_tetrs, mesh2, time_step, virt_nodes);

	// Find contacts mesh2 has with mesh1
	process_mesh(&mesh2_nodes, mesh2, &mesh1_tetrs, mesh1, time_step, virt_nodes);


	return 0;
};

bool CollisionDetector::node_in_intersection(ElasticNode* node, MeshOutline* intersect)
{
	for(int j = 0; j < 3; j++)
		if( (node->coords[j] < intersect->min_coords[j]) || (node->coords[j] > intersect->max_coords[j]) )
			return false;
	return true;
};

void CollisionDetector::clear_contact_data(ElasticNode* node)
{
	node->contact_data->axis_plus[0] = -1;
	node->contact_data->axis_plus[1] = -1;
	node->contact_data->axis_plus[2] = -1;
	node->contact_data->axis_minus[0] = -1;
	node->contact_data->axis_minus[1] = -1;
	node->contact_data->axis_minus[2] = -1;
};

void CollisionDetector::process_direction(ElasticNode* _node, float move, int axis_num, vector<int>* tetrs_vector, TetrMesh_1stOrder* tetrs_mesh, vector<ElasticNode>* virt_nodes)
{
	ElasticNode node = *_node;
	node.coords[0] += move * (node.local_basis)->ksi[axis_num][0];
	node.coords[1] += move * (node.local_basis)->ksi[axis_num][1];
	node.coords[2] += move * (node.local_basis)->ksi[axis_num][2];
	for(int k = 0; k < tetrs_vector->size(); k++) {
		if( tetrs_mesh->point_in_tetr(node.coords[0], node.coords[1], node.coords[2], &((tetrs_mesh->tetrs).at( tetrs_vector->at(k) )) ) ) {
			_node->contact_type = IN_CONTACT;
			// TODO - is it ok to use just this node? Or should we find the one on the border?
			tetrs_mesh->interpolate(&node, &((tetrs_mesh->tetrs).at( tetrs_vector->at(k) )));
			virt_nodes->push_back(node);
			if(move > 0) {
				_node->contact_data->axis_plus[axis_num] = virt_nodes->size()-1;
			} else {
				_node->contact_data->axis_minus[axis_num] = virt_nodes->size()-1;
			}
			break;
		}
	}
};

void CollisionDetector::process_mesh(vector<int>* nodes_vector, TetrMesh_1stOrder* current_mesh, vector<int>* tetrs_vector, TetrMesh_1stOrder* other_mesh, float time_step, vector<ElasticNode>* virt_nodes)
{
	ElasticNode node;

	for(int i = 0; i < nodes_vector->size(); i++) {

		// Clear contact data
		if( (current_mesh->nodes).at( nodes_vector->at(i) ).border_type == BORDER )
			clear_contact_data( &(current_mesh->nodes).at( nodes_vector->at(i) ) );

		node = (current_mesh->nodes).at( nodes_vector->at(i) );

		// TODO - it's bad, we should use default impl, but default one re-randomizes axis...
		float move = time_step * sqrt( ( (node.la) + 2 * (node.mu) ) / (node.rho) );

		// Check all axis
		for(int j = 0; j < 3; j++) {
			// Check positive direction
			process_direction( &(current_mesh->nodes).at( nodes_vector->at(i) ), move, j, tetrs_vector, other_mesh, virt_nodes);

			// Check negative direction
			process_direction( &(current_mesh->nodes).at( nodes_vector->at(i) ), -move, j, tetrs_vector, other_mesh, virt_nodes);
		}

		// TODO - should we check outer normal in addition? It is possible that all axis are out of the neighbour body, but normal is in it.

	}
};

void CollisionDetector::find_elements_in_intersect(TetrMesh_1stOrder* mesh, MeshOutline* intersect, vector<int>* nodes_vector, vector<int>* tetrs_vector)
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
